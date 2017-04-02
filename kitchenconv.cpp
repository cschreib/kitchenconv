// How to compile
// ==============
//
// With GCC:
//   gcc -std=c++11 -O3 kitchenconv.cpp -lstdc++ -o kitchenconv
//

#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
#include <vector>
#include <algorithm>
#include <map>

enum class unit_type {
    none,
    temperature,
    volume,
    weight
};

std::string unit_type_name(unit_type t) {
    switch (t) {
        case unit_type::temperature : return "temperature";
        case unit_type::volume :      return "volume";
        case unit_type::weight :      return "weight";
        default :                     return "unknown";
    }
}

struct unit {
    unit() = default;
    unit(double c, unit_type t) : to_si(c), type(t) {}

    double to_si = 1;
    unit_type type = unit_type::none;
};

inline std::size_t string_distance(const std::string& t, const std::string& u) {
    if (t.size() > u.size()) {
        return string_distance(u, t);
    }

    std::size_t n = t.size();
    std::size_t d = u.size() - t.size();

    std::size_t best_d = -1;
    for (std::size_t k = 0; k < d; ++k) {
        std::size_t td = 0;
        for (std::size_t i = 0; i < n; ++i) {
            if (t[i+k] != u[i]) ++td;
        }

        if (td < best_d) {
            best_d = td;
        }
    }

    return d + best_d;
}

std::map<std::string, double> density_table = {
    {"flour",         0.5283},
    {"butter",        0.9586},
    {"sugar",         0.8453},
    {"salt",          1.1548},
    {"baking-powder", 1.1548},
    {"baking-soda",   0.9337},
    {"baking-powder", 0.7208},
    {"almond-flour",  0.5679},
    {"tomato-paste",  1.1075},
    {"tomato-puree",  1.1075},
    {"rice",          0.8453},
    {"tofu",          1.0480},
    {"parmesan",      0.4227},
    {"oil",           0.9215},
    {"water",         1.0000},
    {"parsley",       0.10566},
    {"basil",         0.10566},
    {"cilantro",      0.10566},
    {"dill",          0.10566},
    {"herbs",         0.10566}
};

std::map<std::string, unit> unit_table = {
    {"kg",   unit{1.0,      unit_type::weight}},
    {"g",    unit{1e-3,     unit_type::weight}},
    {"mg",   unit{1e-6,     unit_type::weight}},
    {"lb",   unit{4.536e-1, unit_type::weight}},
    {"oz",   unit{2.835e-2, unit_type::weight}},
    {"l",    unit{1.0,      unit_type::volume}},
    {"dl",   unit{1e-1,     unit_type::volume}},
    {"cl",   unit{1e-2,     unit_type::volume}},
    {"ml",   unit{1e-3,     unit_type::volume}},
    {"gal",  unit{3.785,    unit_type::volume}},
    {"cup",  unit{2.366e-1, unit_type::volume}},
    {"floz", unit{2.957e-2, unit_type::volume}},
    {"tbs",  unit{1.479e-2, unit_type::volume}},
    {"ts",   unit{4.93e-3,  unit_type::volume}},
    {"c",    unit{1.0,      unit_type::temperature}}, // 1: celcius
    {"f",    unit{0.0,      unit_type::temperature}}  // 0: fahrenheit
};

bool make_unit(unit& u, const std::string& name) {
    auto iter = unit_table.find(name);
    if (iter == unit_table.end()) {
        std::cerr << "error: unknown unit '" << name << "'" << std::endl;
        std::cerr << "note: known units: ";
        std::vector<std::string> units;
        units.reserve(unit_table.size());
        for (auto& v : unit_table) {
            units.push_back(v.first);
        }

        std::sort(units.begin(), units.end(),
            [&](const std::string& s1, const std::string& s2) {
                return string_distance(name, s1) < string_distance(name, s2);
            }
        );

        bool first = true;
        for (auto& v : units) {
            if (!first) std::cerr << ", ";
            std::cerr << v;
            first = false;
        }
        std::cerr << std::endl;
        return false;
    }

    u = iter->second;
    return true;
}

std::string tolower(std::string s) {
    for (auto& c : s) {
        c = std::tolower(c);
    }

    return s;
}

template<typename T>
bool from_string(const std::string& s, T& v) {
    std::istringstream ss(s);
    ss >> v;
    return !ss.fail() && ss.eof();
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cout << "usage examples:" << std::endl;
        std::cout << "  kitchenconv 10 kg to lb" << std::endl;
        std::cout << "  kitchenconv 400 F in C" << std::endl;
        std::cout << "  kitchenconv 1 tbs butter to g" << std::endl;
        std::cout << "  kitchenconv 3 ts of sugar to g" << std::endl;
        std::cout << "  kitchenconv 3/4 cup to ml" << std::endl;
        return 1;
    }

    std::string quantity, unit_from, object_from, unit_to, object_to;
    bool to_found = false;
    for (int i = 1; i < argc; ++i) {
        if (tolower(argv[i]) == "to" || tolower(argv[i]) == "in") {
            if (to_found) {
                std::cerr << "syntax error: multiple 'to' or 'in' not allowed" << std::endl;
                return 1;
            }

            to_found = true;
        } else if (quantity.empty()) {
            quantity = tolower(argv[i]);
        } else if (unit_from.empty()) {
            unit_from = tolower(argv[i]);
        } else if (!to_found && object_from.empty()) {
            if (tolower(argv[i]) != "of") {
                object_from = tolower(argv[i]);
            }
        } else if (to_found && unit_to.empty()) {
            unit_to = tolower(argv[i]);
        } else if (to_found && object_to.empty()) {
            if (tolower(argv[i]) != "of") {
                object_to = tolower(argv[i]);
            }
        } else {
            std::cerr << "syntax error: expected "
                "'<quantity> <unit> [material] to <unit> [material]" << std::endl;
            return 1;
        }
    }

    if (!object_from.empty() && !object_to.empty() && object_to != object_from) {
        std::cerr << "error: cannot convert a quantity of '"
            << object_from << "' into one of '" << object_to << "'" << std::endl;
        return 1;
    }

    std::string object = object_from.empty() ? object_to : object_from;

    unit uf, ut;
    if (!make_unit(uf, unit_from)) return 1;
    if (!make_unit(ut, unit_to))   return 1;

    double quantity_float; {
        auto slash_pos = quantity.find_first_of("/");
        if (slash_pos != quantity.npos) {
            std::string frac_up = quantity.substr(0, slash_pos);
            std::string frac_low = quantity.substr(slash_pos+1);

            std::size_t up = 0, low = 0;
            if (!from_string(frac_up, up) || !from_string(frac_low, low)) {
                std::cerr << "error: could not convert '" << quantity
                    << "' into a number" << std::endl;
                return 1;
            }

            quantity_float = double(up)/double(low);
        } else {
            if (!from_string(quantity, quantity_float)) {
                std::cerr << "error: could not convert '" << quantity
                    << "' into a number" << std::endl;
                return 1;
            }
        }
    }

    if ((uf.type == unit_type::weight && ut.type == unit_type::volume) ||
        (uf.type == unit_type::volume && ut.type == unit_type::weight)) {
        if (object.empty()) {
            std::cerr << "error: converting '" << unit_from << "' (a " <<
                unit_type_name(uf.type) << ") into '" << unit_to << "' (a " <<
                unit_type_name(ut.type) << ") requires knowing the substance "
                "which is converted" << std::endl;
            return 1;
        }

        auto iter = density_table.find(object);
        if (iter == density_table.end()) {
            std::cerr << "error: the density of '" << object << "' is unknown" << std::endl;
            std::cerr << "note: known densities: ";
            std::vector<std::string> densities;
            densities.reserve(density_table.size());
            for (auto& v : density_table) {
                densities.push_back(v.first);
            }

            std::sort(densities.begin(), densities.end(),
                [&](const std::string& s1, const std::string& s2) {
                    return string_distance(object, s1) < string_distance(object, s2);
                }
            );

            bool first = true;
            for (auto& v : densities) {
                if (!first) std::cerr << ", ";
                std::cerr << v;
                first = false;
            }
            std::cerr << std::endl;

            return 1;
        }

        double density_si = iter->second; // kg/L
        if (uf.type == unit_type::volume) {
            uf.type = unit_type::weight;
            uf.to_si *= density_si;
        } else if (ut.type == unit_type::volume) {
            ut.type = unit_type::weight;
            ut.to_si *= density_si;
        }
    }

    if (uf.type != ut.type) {
        std::cerr << "error: cannot convert from '" << unit_from << "' (a " <<
            unit_type_name(uf.type) << ") into '" << unit_to << "' (a " <<
            unit_type_name(ut.type) << ")" << std::endl;
        return 1;
    }

    double result = 0;
    if (uf.type == unit_type::temperature) {
        if (uf.to_si == ut.to_si) {
            result = quantity_float;
        } else if (uf.to_si) {
            // Celcius to Fahrenheit
            result = (9.0/5.0)*quantity_float + 32.0;
        } else {
            // Fahrenheit to Celcius
            result = (5.0/9.0)*(quantity_float - 32.0);
        }
    } else {
        result = quantity_float*uf.to_si/ut.to_si;
    }

    std::cout << "  " << quantity << " " << unit_from << (object.empty() ? "" : " of "+object)
              << " is " << result << " " << unit_to << std::endl;

    return 0;
}
