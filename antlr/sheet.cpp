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
    if (pos.IsValid()) {
        if (table_.find(pos) != table_.end()) {
            return table_.at(pos).get();
        }
        else {
            return nullptr;
        }
    }
    else {
        throw InvalidPositionException("Get Cell: out of range");
    }
}

void Sheet::ClearCell(Position pos) {
    // Size range = GetPrintableSize();
    if (pos.IsValid() /* && !(range.cols < pos.col && range.rows < pos.row) */ ) {
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
    for (int row = 0; row < range.rows; row++) {
        for (int col = 0; col < range.cols; col++) {
            const CellInterface* cell = GetCell({ row, col });
            if (col > 0) {
                output << '\t';
            }
            if (cell != nullptr) {
                output << cell->GetValue();
            }
        }
        output << std::endl;
    }
}

void Sheet::PrintTexts(std::ostream& output) const {
    Size range = GetPrintableSize();
    for (int row = 0; row < range.rows; row++) {
        for (int col = 0; col < range.cols; col++) {
            const CellInterface* cell = GetCell({ row, col });
            if (col > 0) {
                output << '\t';
            }
            if (cell != nullptr) {
                output << cell->GetText();
            }
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
        os << std::get<FormulaError>(value);
    }
    else {
        os << "Unknown Value";
    }
    return os;
}


/*
Разберёмся с ограничениями на производительность. 
Метод GetCell() должен отрабатывать за константное время, в то время как ограничения на время работы для вставки и удаления столбцов/строк самые либеральные. 
Значит, операции произвольного доступа будут вызываться чаще, чем операции вставки.
Наверно, имеет смысл использовать простую структуру данных вроде вектора векторов.
При вставке и удалении строк и столбцов ячейки должны «сдвигаться» на соседние позиции. 
Значит, есть смысл «отвязать» ячейку от её индекса. 
Например, хранить сами ячейки в динамической памяти, а в векторах хранить лишь указатели — желательно умные — на объекты ячеек.
Так вы к тому же экономите память, потому что не выделяете её под пустые ячейки. 
Ведь ваши векторы могут содержать nullptr.
*/