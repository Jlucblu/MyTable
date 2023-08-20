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

class Sheet;

class Cell : public CellInterface {
public:
    Cell(Sheet& sheet);
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    bool IsReferenced() const;
    void ClearCache();
    void UpdDependent(const Position& pos);
    Status GetStatus();
    Type GetType();

private:

    class Impl {
    public:

        virtual Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
        virtual Position GetPosition() const = 0;
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
        Position GetPosition() const override;

    private:
        std::string text_;
        Position pos_;
    };

    class FormulaImpl : public Impl {
    public:

        explicit FormulaImpl(std::string text);
        Value GetValue() const override;
        std::string GetText() const override;
        Position GetPosition() const override;
        virtual void ClearCache();

    private:
        std::unique_ptr<FormulaInterface> formula_ptr_;
        const SheetInterface& sheet_;
        Position pos_;
    };

    std::unique_ptr<Impl> impl_;
    Sheet& sheet_;
    Type type_ = EMPTY;
    Status cache_status_ = NONE;

    std::set<CellInterface*> dependent_;
    std::optional<Value> cache_;
};