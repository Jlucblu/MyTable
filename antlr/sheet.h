#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <unordered_map>

class Sheet : public SheetInterface {
public:
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

private:
    class Hasher {
    public:
        size_t operator() (const Position& pos) const {
            return hasher_(pos.col) + (hasher_(pos.row) * 37);
        }
    private:
        std::hash<int> hasher_;
    };

    std::unordered_map<Position, std::unique_ptr<Cell>, Hasher> table_;
};

std::ostream& operator<<(std::ostream& output, const CellInterface::Value& value);