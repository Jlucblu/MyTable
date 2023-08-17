#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text) {
    if (pos.IsValid()) {
        if (table_.find(pos) != table_.end()) {
            table_[pos]->Set(text);
        }
        else {
            table_[pos] = std::make_unique<Cell>();
            table_[pos]->Set(text);
        }
    }
    else {
        throw InvalidPositionException("Set Cell: out of range");
    }
}

const CellInterface* Sheet::GetCell(Position pos) const {
    return const_cast<Sheet*>(this)->GetCell(pos);
}

CellInterface* Sheet::GetCell(Position pos) {
    if (pos.IsValid() && (table_.find(pos) != table_.end())) {
        return table_.at(pos).get();
    }
    else {
        throw InvalidPositionException("Get Cell: out of range");
    }
}

void Sheet::ClearCell(Position pos) {
    Size range = GetPrintableSize();
    if (pos.IsValid() && !(range.cols < pos.col && range.rows < pos.row)) {
        table_.erase(pos);
    }
    else {
        throw InvalidPositionException("Clear Cell: out of range");
    }
}

Size Sheet::GetPrintableSize() const {
    if (table_.empty()) {
        return Size{ 0, 0 };
    }

    Size num;

    for (const auto& pos : table_) {
        if (num.cols <= pos.first.col) {
            num.cols = pos.first.col + 1;
        }

        if (num.rows <= pos.first.row) {
            num.rows = pos.first.row + 1;
        }
    }

    return num;
}

void Sheet::PrintValues(std::ostream& output) const {
    Size range = GetPrintableSize();
    for (int col = 0; col < range.cols; col++) {
        for (int row = 0; row < range.rows; row++) {
            const CellInterface* cell = GetCell( {row, col} );
            if (row > 0) {
                output << '\t';
            }
            output << cell->GetValue();
        }
        output << std::endl;
    }
}

void Sheet::PrintTexts(std::ostream& output) const {
    Size range = GetPrintableSize();
    for (int col = 0; col < range.cols; col++) {
        for (int row = 0; row < range.rows; row++) {
            const CellInterface* cell = GetCell({ row, col });
            if (row > 0) {
                output << '\t';
            }
            output << cell->GetText();
        }
        output << std::endl;
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}

std::ostream& operator<<(std::ostream& os, const CellInterface::Value& value) {
    if (std::holds_alternative<std::string>(value)) {
        os << std::get<std::string>(value);
    }
    else if (std::holds_alternative<double>(value)) {
        os << std::get<double>(value);
    }
    else if (std::holds_alternative<FormulaError>(value)) {
        os << "Formula Error: " << std::get<FormulaError>(value);
    }
    else {
        os << "Unknown Value";
    }
    return os;
}


/*
��������� � ������������� �� ������������������. 
����� GetCell() ������ ������������ �� ����������� �����, � �� ����� ��� ����������� �� ����� ������ ��� ������� � �������� ��������/����� ����� �����������. 
������, �������� ������������� ������� ����� ���������� ����, ��� �������� �������.
�������, ����� ����� ������������ ������� ��������� ������ ����� ������� ��������.
��� ������� � �������� ����� � �������� ������ ������ ������������ �� �������� �������. 
������, ���� ����� ���������� ������ �� � �������. 
��������, ������� ���� ������ � ������������ ������, � � �������� ������� ���� ��������� � ���������� ����� � �� ������� �����.
��� �� � ���� �� ��������� ������, ������ ��� �� ��������� � ��� ������ ������. 
���� ���� ������� ����� ��������� nullptr.
*/