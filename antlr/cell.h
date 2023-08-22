#pragma once

#include "common.h"
#include "formula.h"
#include <optional>
#include <set>

enum Type {
    EMPTY,
    FORMULA,
    TEXT
};

class Cell : public CellInterface {
public:
    Cell(SheetInterface& sheet);
    ~Cell();

    void Set(std::string text, Position pos);
    void CheckCyclic(const Position& pos, const std::vector<Position>& cells);
    void UpdDependent(const Position& current_pos, const Position& dependent_pos);
    void RemoveDependencies();
    void Clear();
    void ClearCache();


    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

private:

    class Impl {
    public:

        virtual Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
        virtual std::vector<Position> GetReferencedCells() const;
        virtual void ClearCache();

        virtual ~Impl() = default;
    };

    class EmptyImpl : public Impl {
    public:

        Value GetValue() const override;
        std::string GetText() const override;
    };

    class TextImpl : public Impl {
    public:

        explicit TextImpl(std::string text);
        Value GetValue() const override;
        std::string GetText() const override;

    private:
        std::string text_;
    };

    class FormulaImpl : public Impl {
    public:

        explicit FormulaImpl(std::string text, SheetInterface& sheet);
        Value GetValue() const override;
        std::string GetText() const override;
        void ClearCache() override;
        std::vector<Position> GetReferencedCells() const override;

    private:
        std::unique_ptr<FormulaInterface> formula_ptr_;
        SheetInterface& sheet_prt_;
        mutable std::optional<FormulaInterface::Value> cache_;
    };

    std::unique_ptr<Impl> CreateImpl(std::string text);


    std::unique_ptr<Impl> impl_;
    SheetInterface& sheet_;
    std::set<Cell*> cells_dependent_on_this_;
    std::set<Cell*> cells_this_depends_on_;

    Type type_ = EMPTY;
};