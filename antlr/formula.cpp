#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <set>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << fe.ToString();
}

namespace {
class Formula : public FormulaInterface {
public:
// Реализуйте следующие методы:
    explicit Formula(std::string expression)
        :ast_(ParseFormulaAST(expression)) {}

    Value Evaluate(const SheetInterface& args) const override {
        try {
            return ast_.Execute(args);
        }
        catch (const FormulaError& e) {
            return e;
        }
    }

    std::string GetExpression() const override {
        std::stringstream ss;
        ast_.PrintFormula(ss);
        return ss.str();
    }

    std::vector<Position> GetReferencedCells() const override {
        std::set<Position> cells;
        for (Position pos : ast_.GetCells()) {
            if (pos.IsValid()) {
                cells.insert(pos);
            }
        }

        std::vector<Position> result(cells.begin(), cells.end());

        return result;
    }

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}

FormulaError::FormulaError(Category category)
    : category_(category)
{}

FormulaError::Category FormulaError::GetCategory() const {
    return category_;
}

bool FormulaError::operator==(FormulaError rhs) const {
    return category_ == rhs.category_;
}

std::string_view FormulaError::ToString() const {
    switch (category_) {
    case Category::Ref:
        return "#REF!";
    case Category::Value:
        return "#VALUE!";
    case Category::Div0:
        return "#DIV/0!";
    default:
        return "";
    }
}