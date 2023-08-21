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

enum Status {
    DIRTY,
    CLEAN,
    NONE
};

class Cell : public CellInterface {
public:
    Cell(SheetInterface& sheet);
    ~Cell();

    void Set(std::string text, Position pos);
    void CheckCyclic(const Position& pos, const std::vector<Position>& cells);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    //bool IsReferenced() const;
    //void ClearCache();
    //void UpdDependent(const Position& pos);

    //Status GetStatus();
    //Type GetType();

private:

    class Impl {
    public:

        virtual Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
        //virtual Position GetPosition() const = 0;
        //virtual void ClearCache();
        virtual std::vector<Position> GetReferencedCells() const;

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
        //virtual void ClearCache();
        std::vector<Position> GetReferencedCells() const override;

    private:
        std::unique_ptr<FormulaInterface> formula_ptr_;
        SheetInterface& sheet_prt_;
        mutable std::optional<Value> cache_;
    };

    std::unique_ptr<Impl> CreateImpl(std::string text);


    std::unique_ptr<Impl> impl_;
    SheetInterface& sheet_;
    /*std::set<CellInterface*> dependent_;*/

    Type type_ = EMPTY;
    //Status cache_status_ = NONE;
};