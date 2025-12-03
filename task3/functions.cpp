#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <initializer_list>
#include <memory>
#include <unordered_map>
#include <iostream>


class TFunction;
class Factory;
using TFunction_ptr = std::shared_ptr<TFunction>;

static const std::unordered_map<std::string, int> type2int = {
    {"ident",      0},
    {"const",      1},
    {"power",      2},
    {"exp",        3},
    {"polynomial", 4},
};


class TFunction {
public:
    virtual ~TFunction() = default;

    virtual double evaluate(double x) const = 0;
    virtual double derivate(double x) const = 0;

    virtual std::string ToString() const = 0;

    double operator()(double x) const {
        return evaluate(x);
    }

    double GetDeriv(double x) const {
        return derivate(x);
    }

    std::string to_string() const {
        return ToString();
    }
};

class Identity : public TFunction {
public:
    double evaluate(double x) const override {
        return x;
    }

    double derivate(double x) const override {
        return 1.0;
    }

    std::string ToString() const override {
        return "x";
    }
};

class Const : public TFunction {
private:
    double val;
public:
    explicit Const(double v) : val(v) {}

    double evaluate(double x) const override {
        return val;
    }

    double derivate(double x) const override {
        return 0.0;
    }

    std::string ToString() const override {
        return std::to_string(val);
    }
};

class Power : public TFunction {
private:
    double power;
public:
    explicit Power(double p) : power(p) {}

    double evaluate(double x) const override {
        if (x == 0.0 && power <= 0.0) {
            throw std::invalid_argument("Division by 0.");
        }
        return std::pow(x, power);
    }

    double derivate(double x) const override {
        if (x == 0.0 && power - 1.0 <= 0.0) {
            throw std::invalid_argument("Division by 0.");
        }
        return power * std::pow(x, power - 1.0);
    }

    std::string ToString() const override {
        return "x^" + std::to_string(power);
    }
};

class Exponent : public TFunction {
public:
    explicit Exponent() {}

    double evaluate(double x) const override {
        return std::exp(x);
    }

    double derivate(double x) const override {
        return std::exp(x);
    }

    std::string ToString() const override {
        return "e^x";
    }
};

class Polynomial : public TFunction {
private:
    std::vector<double> coefs; // coefs[i] — коэффициент при x^i
public:
    explicit Polynomial(const std::vector<double> &vec) : coefs(vec) {}

    double evaluate(double x) const override {
        double res = 0.0;
        int power = 0;
        for (auto &i : coefs) {
            res += i * std::pow(x, power);
            ++power;
        }
        return res;
    }

    double derivate(double x) const override {
        double res = 0.0;
        int power = 0;
        for (auto &i : coefs) {
            if (power != 0) {
                res += i * power * std::pow(x, power - 1);
            }
            ++power;
        }
        return res;
    }

    std::string ToString() const override {
        if (coefs.empty()) {
            return "0.0";
        }
        if (coefs.size() == 1) {
            return std::to_string(coefs[0]);
        }
        std::string res;
        int power = static_cast<int>(coefs.size()) - 1;
        for (auto it = coefs.rbegin(); it != coefs.rend(); ++it) {
            if (*it == 0.0) { continue;} 
            if (power != 0) {
                res += *it > 0 ? " + " + std::to_string(*it) + "x^" + std::to_string(power): " - " + std::to_string(std::abs(*it)) + "x^" + std::to_string(power);
            } else {
                res += " + " + std::to_string(*it);
            }
            --power;
        }
        // удалить первый " + "
        if (res.size() >= 3 && res[1] == '+') {
            res.erase(0, 3);
        }
        return res;
    }
};

class Addition : public TFunction {
private:
    TFunction_ptr a;
    TFunction_ptr b;
public:
    explicit Addition(TFunction_ptr a_, TFunction_ptr b_) : a(std::move(a_)), b(std::move(b_)) {}

    double evaluate(double x) const override {
        return a->evaluate(x) + b->evaluate(x);
    }

    double derivate(double x) const override {
        return a->derivate(x) + b->derivate(x);
    }

    std::string ToString() const override {
        return "(" + a->ToString() + ") + (" + b->ToString() + ")";
    }
};

class Subtraction : public TFunction {
private:
    TFunction_ptr a;
    TFunction_ptr b;
public:
    explicit Subtraction(TFunction_ptr a_, TFunction_ptr b_) : a(std::move(a_)), b(std::move(b_)) {}

    double evaluate(double x) const override {
        return a->evaluate(x) - b->evaluate(x);
    }

    double derivate(double x) const override {
        return a->derivate(x) - b->derivate(x);
    }

    std::string ToString() const override {
        return "(" + a->ToString() + ") - (" + b->ToString() + ")";
    }
};

class Multiplication : public TFunction {
private:
    TFunction_ptr a;
    TFunction_ptr b;
public:
    explicit Multiplication(TFunction_ptr a_, TFunction_ptr b_) : a(std::move(a_)), b(std::move(b_)) {}

    double evaluate(double x) const override {
        return a->evaluate(x) * b->evaluate(x);
    }

    double derivate(double x) const override {
        // (a*b)' = a'b + b'a
        return a->derivate(x) * b->evaluate(x) + b->derivate(x) * a->evaluate(x);
    }

    std::string ToString() const override {
        return "(" + a->ToString() + ") * (" + b->ToString() + ")";
    }
};

class Division : public TFunction {
private:
    TFunction_ptr a;
    TFunction_ptr b;
public:
    explicit Division(TFunction_ptr a_, TFunction_ptr b_) : a(std::move(a_)), b(std::move(b_)) {}

    double evaluate(double x) const override {
        double denom = b->evaluate(x);
        if (denom == 0.0) {
            throw std::invalid_argument("Division by 0.");
        }
        return a->evaluate(x) / denom;
    }

    double derivate(double x) const override {
        double denom = b->evaluate(x);
        if (denom == 0.0) {
            throw std::invalid_argument("Division by 0.");
        }
        double num = a->derivate(x) * denom - b->derivate(x) * a->evaluate(x);
        return num / std::pow(denom, 2);
    }

    std::string ToString() const override {
        return "(" + a->ToString() + ") / (" + b->ToString() + ")";
    }
};

TFunction_ptr operator+(TFunction_ptr a, TFunction_ptr b) {
    return std::make_shared<Addition>(std::move(a), std::move(b));
}

TFunction_ptr operator-(TFunction_ptr a, TFunction_ptr b) {
    return std::make_shared<Subtraction>(std::move(a), std::move(b));
}

TFunction_ptr operator*(TFunction_ptr a, TFunction_ptr b) {
    return std::make_shared<Multiplication>(std::move(a), std::move(b));
}

TFunction_ptr operator/(TFunction_ptr a, TFunction_ptr b) {
    return std::make_shared<Division>(std::move(a), std::move(b));
}

// Фабрика функций
class Factory {
public:
    TFunction_ptr create(const std::string& type, double param = 0.0) {
        auto it = type2int.find(type);
        if (it == type2int.end()) {
            throw std::logic_error("Unknown function type: " + type);
        }
        switch (it->second) {
            case 0: // ident
                return std::make_shared<Identity>();
            case 1: // const
                return std::make_shared<Const>(param);
            case 2: // power
                return std::make_shared<Power>(param);
            case 3: // exp
                return std::make_shared<Exponent>();
            default:
                throw std::logic_error("Unsupported function type for this overload: " + type);
        }
    }

    TFunction_ptr create(const std::string& type, const std::vector<double>& coefs) {
        auto it = type2int.find(type);
        if (it == type2int.end()) {
            throw std::logic_error("Unknown function type: " + type);
        }
        switch (it->second) {
            case 4: // polynomial
                return std::make_shared<Polynomial>(coefs);
            default:
                throw std::logic_error("Unsupported function type for polynomial overload: " + type);
        }
    }
};

// eq    — выражение f(x)
// lr    — шаг градиентного спуска 
double find_equation_root(TFunction_ptr &eq,
                          double x0 = 0.0,
                          int iters = 100000,
                          double lr = 1e-3)
{
    for (int i = 0; i < iters; ++i) {
        double fx  = eq->evaluate(x0);
        double dfx = eq->derivate(x0);

        x0 = x0 - lr * fx * dfx;
    }
    return x0;
}
