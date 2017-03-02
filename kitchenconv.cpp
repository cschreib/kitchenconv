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

enum class unit_type {
    temperature,
    volume,
    weight
};

std::string unit_type_name(unit_type t) {
    switch (t) {
        case unit_type::temperature : return "temperature";
        case unit_type::volume :      return "volume";
        case unit_type::weight :      return "weight";
    }
}

struct unit {
    double to_si = 1;
    unit_type type;
};

bool make_unit(unit& u, const std::string& name) {
    if (name == "kg") {
        u.to_si = 1;
        u.type = unit_type::weight;
    } else if (name == "g") {
        u.to_si = 1e-3;
        u.type = unit_type::weight;
    } else if (name == "mg") {
        u.to_si = 1e-6;
        u.type = unit_type::weight;
    } else if (name == "lb") {
        u.to_si = 4.536e-1;
        u.type = unit_type::weight;
    } else if (name == "oz") {
        u.to_si = 2.835e-2;
        u.type = unit_type::weight;
    } else if (name == "l") {
        u.to_si = 1;
        u.type = unit_type::volume;
    } else if (name == "dl") {
        u.to_si = 1e-1;
        u.type = unit_type::volume;
    } else if (name == "cl") {
        u.to_si = 1e-2;
        u.type = unit_type::volume;
    } else if (name == "ml") {
        u.to_si = 1e-3;
        u.type = unit_type::volume;
    } else if (name == "gal") {
        u.to_si = 3.785;
        u.type = unit_type::volume;
    } else if (name == "cup") {
        u.to_si = 2.366e-1;
        u.type = unit_type::volume;
    } else if (name == "floz") {
        u.to_si = 2.957e-2;
        u.type = unit_type::volume;
    } else if (name == "tbs") {
        u.to_si = 1.479e-2;
        u.type = unit_type::volume;
    } else if (name == "ts") {
        u.to_si = 4.93e-3;
        u.type = unit_type::volume;
    } else if (name == "c") {
        u.to_si = 1; // 1: celcius
        u.type = unit_type::temperature;
    } else if (name == "f") {
        u.to_si = 0; // 0: fahrenheit
        u.type = unit_type::temperature;
    } else {
        std::cerr << "error: unknown unit '" << name << "'" << std::endl;
        return false;
    }

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

        double density_si = 0; // kg/L
        if (object == "flour") {
            density_si = 0.5283;
        } else if (object == "butter") {
            density_si = 0.9586;
        } else if (object == "sugar") {
            density_si = 0.8453;
        } else if (object == "salt") {
            density_si = 1.1548;
        } else if (object == "parsley" || object == "basil" || object == "cilantro" ||
            object == "dill" || object == "herbs") {
            density_si = 0.10566;
        } else {
            std::cerr << "error: the density of '" << object << "' is unknown" << std::endl;
            return 1;
        }

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
