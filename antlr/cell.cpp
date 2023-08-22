#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

Cell::Cell(SheetInterface& sheet) 
    : impl_(std::make_unique<EmptyImpl>())
    , sheet_(sheet) {
}
Cell::~Cell() = default;

void Cell::Set(std::string text, Position pos) {
    ClearCache();
    RemoveDependencies();
    auto impl = CreateImpl(text);
    auto cells = impl->GetReferencedCells();

    if (type_ == FORMULA) {
        CheckCyclic(pos, cells);
    }

    impl_ = std::move(impl);

    for (auto& cell : cells) {
        UpdDependent(pos, cell);
    }
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const { 
    return impl_->GetValue(); 
}
std::string Cell::GetText() const { 
    return impl_->GetText(); 
}

Cell::Value Cell::EmptyImpl::GetValue() const { 
    return ""; 
}

std::string Cell::EmptyImpl::GetText() const { 
    return ""; 
}

Cell::TextImpl::TextImpl(std::string text) : text_(std::move(text)) {}

Cell::Value Cell::TextImpl::GetValue() const {
    if (text_.empty()) {
        throw std::logic_error("Get Value: it is empty impl, not text");

    }
    else if (text_.at(0) == ESCAPE_SIGN) {
        return text_.substr(1);

    }
    else {
        return text_;
    }
}

std::string Cell::TextImpl::GetText() const { return text_; }

Cell::FormulaImpl::FormulaImpl(std::string text, SheetInterface& sheet) 
    : formula_ptr_(ParseFormula(text))
    , sheet_prt_(sheet)
{}

Cell::Value Cell::FormulaImpl::GetValue() const {
    if (!cache_.has_value()) {
        cache_ = formula_ptr_->Evaluate(sheet_prt_);
    }

    if (std::holds_alternative<double>(cache_.value())) {
        return std::get<double>(cache_.value());
    }
    else {
        return std::get<FormulaError>(cache_.value());
    }
}

std::string Cell::FormulaImpl::GetText() const { return FORMULA_SIGN + formula_ptr_->GetExpression(); }

std::vector<Position> Cell::GetReferencedCells() const {
    return impl_->GetReferencedCells();
}

std::vector<Position> Cell::Impl::GetReferencedCells() const {
    return {};
}

std::vector<Position> Cell::FormulaImpl::GetReferencedCells() const {
    return formula_ptr_->GetReferencedCells();
}

std::unique_ptr<Cell::Impl> Cell::CreateImpl(std::string text) {
    std::unique_ptr<Cell::Impl> impl;
    if (text.empty()) {
        type_ = EMPTY;
        impl = std::make_unique<EmptyImpl>();
    }
    else if (text.size() > 1 && text[0] == FORMULA_SIGN) {
        type_ = FORMULA;
        impl = std::make_unique<FormulaImpl>(std::move(text.substr(1)), sheet_);
    }
    else {
        type_ = TEXT;
        impl = std::make_unique<TextImpl>(std::move(text));
    }

    return impl;
}

void Cell::CheckCyclic(const Position& pos, const std::vector<Position>& cells) {
    for (const auto& cell : cells) {
        if (pos == cell) {
            throw CircularDependencyException("Circular dependency detected at position : " + pos.ToString());
        }
        
        auto current_cell = sheet_.GetCell(cell);
        if (current_cell == nullptr) {
            sheet_.SetCell(cell, {});
        }

        if (current_cell != nullptr) {
            CheckCyclic(pos, current_cell->GetReferencedCells());
        }
    }
}

void Cell::UpdDependent(const Position& current_pos, const Position& dependent_pos) {
    Cell* current_cell = dynamic_cast<Cell*>(sheet_.GetCell(current_pos));
    Cell* dependent_cell = dynamic_cast<Cell*>(sheet_.GetCell(dependent_pos));
    dependent_cell->cells_dependent_on_this_.insert(current_cell);
    cells_this_depends_on_.insert(dependent_cell);
}

void Cell::Impl::ClearCache() {}

void Cell::FormulaImpl::ClearCache() {
    cache_.reset();
}

void Cell::ClearCache() {
    impl_->ClearCache();
    for (auto dep_cell : cells_dependent_on_this_) {
        dep_cell->ClearCache();
    }
}

void Cell::RemoveDependencies() {
    for (auto dep_cell : cells_this_depends_on_) {
        dep_cell->cells_dependent_on_this_.erase(this);
    }
    cells_this_depends_on_.clear();
}