#include "common.h"

#include <cctype>
#include <sstream>
#include <charconv>
#include <tuple>
#include <cmath>

const int LETTERS = 26;
const int MAX_POSITION_LENGTH = 17;
const int MAX_POS_LETTER_COUNT = 3;
constexpr int alphabetSize = 26;
constexpr int asciiOffset = 64;

const Position Position::NONE = {-1, -1};

// Реализуйте методы:
bool Position::operator==(const Position rhs) const {
    return row == rhs.row && col == rhs.col;
}

bool Position::operator<(const Position rhs) const {
    return std::tie(row, col) < std::tie(rhs.row, rhs.col);
}

bool Position::IsValid() const {
	return (row >= 0 && col >= 0) && (row < MAX_ROWS && col < MAX_COLS);
}

std::string Position::ToString() const {
    if (!IsValid()) {
        return "";
    }

    std::string result;
    result.reserve(MAX_POSITION_LENGTH);
    int c = col;
    while (c >= 0) {
        result.insert(result.begin(), 'A' + c % LETTERS);
        c = c / LETTERS - 1;
    }

    result += std::to_string(row + 1);

    return result;
}

Position Position::FromString(std::string_view str) {
    int letterCount = 0;
    for (auto letter : str) {
        if (letter >= 'A' && letter <= 'Z')
            ++letterCount;
        else
            break;
    }

    for (int i = letterCount; i < static_cast<int>(str.size()); ++i) {
        if (!(str[i] >= '0' && str[i] <= '9')) {
            return NONE;
        }
    }

    int value_row = 0;
    const auto& part_row = str.substr(letterCount, str.size() - letterCount);
    std::from_chars(part_row.data(), part_row.data() + part_row.size(), value_row);

    int value_column = 0;
    const auto& part_column = str.substr(0, letterCount);
    for (int i = static_cast<int>(part_column.size() - 1), j = 0; i >= 0; --i, ++j) {
        value_column += (part_column[i] - asciiOffset) * std::pow(alphabetSize, j);
    }

    Position result = { value_row - 1,  value_column - 1 };
    if (!result.IsValid()) {
        return Position::NONE;
    }
    return result;
}