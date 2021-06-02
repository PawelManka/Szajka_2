
#include "TSP.hpp"

#include <algorithm>
#include <stack>
#include <optional>
#include <iostream>
#include <set>

std::ostream& operator<<(std::ostream& os, const CostMatrix& cm) {
    for (std::size_t r = 0; r < cm.size(); ++r) {
        for (std::size_t c = 0; c < cm.size(); ++c) {
            const auto& elem = cm[r][c];
            os << (is_inf(elem) ? "INF" : std::to_string(elem)) << " ";
        }
        os << "\n";
    }
    os << std::endl;

    return os;
}


/**
 * Create path from unsorted path and last 2x2 cost matrix.
 * @return The vector of consecutive vertex.
 */
path_t StageState::get_path() {
    auto unsorted_path = get_unsorted_path();
    for (std::size_t i = 0; i < matrix_.size(); ++i){
        for (std::size_t j = 0; j < matrix_.size(); ++j){
            if (matrix_[i][j] != INF){
                unsorted_path.push_back(vertex_t{i, j});                    // dodanie do nieposortowanej ścieżki pozostałych przejść
            }
        }
    }
    unsorted_path_t sorted_unsorted = {unsorted_path[0]};                       // chce sortować liste unsorted

    for (std::size_t sort_uns_iter = 0; sort_uns_iter < unsorted_path.size() - 1; sort_uns_iter++){
        for (std::size_t i = 1; i < unsorted_path.size(); ++i){                 // pierwszy element biorę z inicjalizacji
            vertex_t vertex = unsorted_path[i];                                         // biore następny vertex
            if (vertex.row == sorted_unsorted[sort_uns_iter].col){                  // sprawdzam czy kolumna poprzedniego wyrazu == wiersz nastepnego
                sorted_unsorted.push_back(vertex);
                break;
            }
        }
    }
    std::vector<std::size_t> final_path;                                        // tworze końcową ścieżke
    for (const auto& vertex: sorted_unsorted){
        final_path.push_back(vertex.row);                              // uzupełniam o etykiety wierzchołków
    }
    return final_path;                                                          // zwracam ścieżke
}

/**
 * Get minimum values from each row and returns them.
 * @return Vector of minimum values in row.
 */
std::vector<cost_t> CostMatrix::get_min_values_in_rows() const {
    std::vector<cost_t> min_val_row = {};                              // tworzę wektor minimalnych wartości
    for (const auto& row : matrix_){                                    // pętla po kolejnych wierszach
        cost_t min_value = INF;
        for (const auto& elem : row){                                   // pętla po kolumnach
            if (elem < min_value){                                      // sprawdzenie czy aktualna najmniejsza wartość jest większa od elementu
                min_value = elem;
            }
        }
        min_val_row.push_back(min_value);                               // dodanie elementu do wektora
    }
    return min_val_row;
}

/**
 * Reduce rows so that in each row at least one zero value is present.
 * @return Sum of values reduced in rows.
 */
cost_t CostMatrix::reduce_rows() {                          /* redukuje z każdego wiersza, nie wiem czy ma znaczenie, że odejmuje też od INF */
    std::vector<cost_t> m = get_min_values_in_rows();       // znajduję wektor najmniejszych wartości
    cost_t reduced_sum = 0;
    for (std::size_t k= 0; k < matrix_.size(); ++k){               // przejście po wierszach
        for (std::size_t i = 0; i < matrix_.size(); ++i){          // przejście po kolumnach
            if (matrix_[k][i] != INF) {
                matrix_[k][i] -= m[k];                          // odjęcie minimalnej wartości od miejsca w macierzy
            }

        }
        if (m[k] != INF) {
            reduced_sum += m[k];                                // dodanie do sumy ogólnej wartości odejmowanej w danym wierszu
        }
    }
    return reduced_sum;
}


/**
 * Get minimum values from each column and returns them.
 * @return Vector of minimum values in columns.
 */
 
std::vector<cost_t> CostMatrix::get_min_values_in_cols() const {
    std::vector<cost_t> min_values_col = {};
    for (std::size_t i = 0; i < matrix_.size(); ++i) {              // przechodze po odpowiednim indeksie kolumny
        cost_t min_value = INF;                                     // zmiana wartości minimalnej na INF
        for (const auto& row : matrix_) {                           // przejscie po elementach kolumny (w dół)
            if (row[i] < min_value) {
                min_value = row[i];                                 // nadpisanie minimalnej wartości
            }
        }
        min_values_col.push_back(min_value);
    }
    return min_values_col;
}

/**
 * Reduces rows so that in each column at least one zero value is present.
 * @return Sum of values reduced in columns.
 */
cost_t CostMatrix::reduce_cols() {
    std::vector<cost_t> minn = get_min_values_in_cols();
    cost_t reduced_sum = 0;
    for(std::size_t i = 0; i < matrix_.size(); ++i)                    // Wiersze
    {
        for(std::size_t j = 0; j < matrix_.size(); ++j)                // Kolumny
        {
            if (matrix_[j][i] != INF) {
                matrix_[j][i] -= minn[i];                       // przejscie po wszystkich wierszach w jednej kolumnie
            }

        }                                                       // odejmuje wartosc minimalna dla tej kolumny
        if (minn[i] != INF){
            reduced_sum += minn[i];
        }

    }
    return reduced_sum;
}

/**
 * Get the cost of not visiting the vertex_t (@see: get_new_vertex())
 * @param row
 * @param col
 * @return The sum of minimal values in row and col, excluding the intersection value.
 */
cost_t CostMatrix::get_vertex_cost(std::size_t row, std::size_t col) const {
    //@TODO naprawić!

    std::vector<int> values_in_row = {};
    std::vector<int> values_in_col = {};

    for(std::size_t row_it = 0; row_it < matrix_.size(); row_it++){ //iterowanie po wierszach, żeby otrzymać kolumnę

        if(row_it != row and matrix_[row_it][col] != INF){
            values_in_col.push_back(matrix_[row_it][col]);
        }
    }
    for(std::size_t column_it = 0; column_it < matrix_[row].size(); column_it++){ //iterowanie po kolumnach, żeby otrzymać wiersz

        if(column_it == col or matrix_[row][column_it] == INF){
            continue;
        }else{
            values_in_row.push_back(matrix_[row][column_it]);
        }
    }

    if (values_in_col.size() != 0 and values_in_row.size() != 0){
        cost_t min_val_in_col = values_in_col[0];
        cost_t min_val_in_row = values_in_row[0];
        for (std::size_t col_it=0; col_it<values_in_col.size(); col_it++){ //szuka najmnieszej wartości w wektorze
            if(min_val_in_col > values_in_col[col_it]){
                min_val_in_col = values_in_col[col_it];
            }
        }

        for (std::size_t row_it=0; row_it<values_in_row.size(); row_it++){ //szuka najmnieszej wartości w wektorze
            if(min_val_in_row > values_in_row[row_it]){
                min_val_in_row = values_in_row[row_it];
            }
        }

        return min_val_in_col + min_val_in_row;
    }else{
        return 0;
    }
}


/* PART 2 */

/**
 * Choose next vertex to visit:
 * - Look for vertex_t (pair row and column) with value 0 in the current cost matrix.
 * - Get the vertex_t cost (calls get_vertex_cost()).
 * - Choose the vertex_t with maximum cost and returns it.
 * @return The coordinates of the next vertex.
 */
NewVertex StageState::choose_new_vertex() {             // zwraca współrzędne wierzchołków

    unsorted_path_t uns_path = get_unsorted_path();     // biorę nieposortowaną ścieżkę (dotychczasową) tworze wektor wierzchołkow ktore wystąpiły
    std::set<std::size_t> vert_coordinates;
    for (const vertex_t& el: uns_path){                         // aktualizacja zbiorow
        vert_coordinates.insert(el.row);
        vert_coordinates.insert(el.col);
    }

    std::vector<size_t> row_ids = {};               // wektor pierwszych współrzędnych wierzchołków z wartością 0
    std::vector<size_t> col_ids = {};               // wektor drugich współrzędnych wierzchołków z wartpścią 0
    for (std::size_t row = 0; row < matrix_.size(); ++row){
        for (std::size_t col = 0; col < matrix_.size(); ++col){
            if (matrix_[row][col] == 0){                             // znalezc wszystkie wierzchołki z wartością 0
                if (uns_path.size() > 0){
                    auto iterator_row = vert_coordinates.find(row);                   // jesli element z 1. wspolrzednych nie znajduje sie w zbiorze 2. wsp jest do zablokowania
                    auto iterator_col = vert_coordinates.find(col);
                    if (iterator_row != std::end(vert_coordinates) or iterator_col != std::end(vert_coordinates)){
                        row_ids.push_back(row);                               // potem znalezc sumę minimalnych wartości w wierszu i kolumnie
                        col_ids.push_back(col);                               // potem wyznaczyć gdzie ta wartość jest największa, ją zwrócić
                    }
                }
                else{
                    row_ids.push_back(row);                               // potem znalezc sumę minimalnych wartości w wierszu i kolumnie
                    col_ids.push_back(col);                               // potem wyznaczyć gdzie ta wartość jest największa, ją zwrócić
                }

            }
        }
    }

    cost_t max_value = -INF;                             // do znalezienia maxymalnej wartości
    std::size_t row = 0;
    std::size_t col = 0;                                 // współrzędne do zwrócenia
    for (std::size_t vertex_zero = 0; vertex_zero < row_ids.size(); ++vertex_zero){                    // przejście po wierzchołkach zerowych
        cost_t value = matrix_.get_vertex_cost(row_ids[vertex_zero], col_ids[vertex_zero]);     // znalezienie wartości
        if (value > max_value){
            max_value = value;
            row = row_ids[vertex_zero];
            col = col_ids[vertex_zero];
        }
    }
    return NewVertex{vertex_t{row, col}, max_value};             // stworzenie nowego wierzchołka
}

/**
 * Update the cost matrix with the new vertex.
 * @param new_vertex
 */
void StageState::update_cost_matrix(vertex_t new_vertex) {
    unsorted_path_t uns_path = get_unsorted_path();     // biorę nieposortowaną ścieżkę (dotychczasową), żeby zabronić cykli
    cost_matrix_t new_matrix = matrix_.get_matrix();

    std::set<std::size_t> row_coordinates;                      // zbiór wszystkich 1. wpsolrzednych
    std::set<std::size_t> col_coordinates;                      // zbior wszystkich 2. wspolrzednych
    std::size_t col_to_block = INF;
    std::size_t row_to_block = INF;

    for (const vertex_t& el: uns_path){                         // aktualizacja zbiorow
        row_coordinates.insert(el.row);
        col_coordinates.insert(el.col);
    }
    for (const std::size_t& el: row_coordinates){
        auto iterator = col_coordinates.find(el);                   // jesli element z 1. wspolrzednych nie znajduje sie w zbiorze 2. wsp jest do zablokowania
        if (iterator == std::end(col_coordinates)){
            col_to_block = el;
        }
    }
    for (const auto& el: col_coordinates){
        auto iterator = row_coordinates.find(el);                   // ^ to samo tylko ze zbioru 2 wspolrzednych szukam elementu który nie wystepuje w 1. zbiorze
        if (iterator == std::end(row_coordinates)){
            row_to_block = el;
        }
    }
    if (row_to_block != INF and col_to_block != INF) {
        new_matrix[row_to_block][col_to_block] = INF;               // blokowanie wierzchołka przeciw cyklom
    }

    std::size_t row_deletion = new_vertex.row;
    std::size_t col_deletion = new_vertex.col;
    new_matrix[col_deletion][row_deletion] = INF;               // zabronienie przejscia powrotnego

    for(std::size_t row_it = 0; row_it < new_matrix.size(); row_it++){ //iterowanie po wierszach, żeby otrzymać kolumnę
        new_matrix[row_it][col_deletion] = INF;
    }
    for(std::size_t column_it = 0; column_it < new_matrix[row_deletion].size(); column_it++){ //iterowanie po kolumnach, żeby otrzymać wiersz

        new_matrix[row_deletion][column_it] = INF;
    }

    matrix_ = CostMatrix(new_matrix);
}

/**
 * Reduce the cost matrix.
 * @return The sum of reduced values.
 */
cost_t StageState::reduce_cost_matrix()
{


    auto new_cost = 0;
    new_cost += matrix_.reduce_rows();
    std::size_t a = 0;

    for(std::size_t it=0; it < matrix_.get_min_values_in_cols().size(); it++){
        auto vector = matrix_.get_min_values_in_cols();
        if(matrix_.get_min_values_in_cols()[it] == 0){
            a = a + 1;
        }
    }

    if (a != matrix_.get_min_values_in_cols().size()){
        new_cost += matrix_.reduce_cols();

    }
    return new_cost;
}

/**
 * Given the optimal path, return the optimal cost.
 * @param optimal_path
 * @param m
 * @return Cost of the path.
 */
cost_t get_optimal_cost(const path_t& optimal_path, const cost_matrix_t& m) {
    cost_t cost = 0;

    for (std::size_t idx = 1; idx < optimal_path.size(); ++idx) {
        cost += m[optimal_path[idx - 1]][optimal_path[idx]];
    }

    // Add the cost of returning from the last city to the initial one.
    cost += m[optimal_path[optimal_path.size() - 1]][optimal_path[0]];

    return cost;
}

/**
 * Create the right branch matrix with the chosen vertex forbidden and the new lower bound.
 * @param m
 * @param v
 * @param lb
 * @return New branch.
 */
StageState create_right_branch_matrix(cost_matrix_t m, vertex_t v, cost_t lb) {
    CostMatrix cm(m);
    cm[v.row][v.col] = INF;
    return StageState(cm, {}, lb);
}

/**
 * Retain only optimal ones (from all possible ones).
 * @param solutions
 * @return Vector of optimal solutions.
 */
tsp_solutions_t filter_solutions(tsp_solutions_t solutions) {
    cost_t optimal_cost = INF;
    for (const auto& s : solutions) {
        optimal_cost = (s.lower_bound < optimal_cost) ? s.lower_bound : optimal_cost;
    }

    tsp_solutions_t optimal_solutions;
    std::copy_if(solutions.begin(), solutions.end(),
                 std::back_inserter(optimal_solutions),
                 [&optimal_cost](const tsp_solution_t& s) { return s.lower_bound == optimal_cost; }
    );

    return optimal_solutions;
}

/**
 * Solve the TSP.
 * @param cm The cost matrix.
 * @return A list of optimal solutions.
 */
tsp_solutions_t solve_tsp(const cost_matrix_t& cm) {

    StageState left_branch(cm);

    // The branch & bound tree.
    std::stack<StageState> tree_lifo;

    // The number of levels determines the number of steps before obtaining
    // a 2x2 matrix.
    std::size_t n_levels = cm.size() - 2;

    tree_lifo.push(left_branch);   // Use the first cost matrix as the root.

    cost_t best_lb = INF;
    tsp_solutions_t solutions;

    while (!tree_lifo.empty()) {

        left_branch = tree_lifo.top();
        tree_lifo.pop();

        while (left_branch.get_level() != n_levels && left_branch.get_lower_bound() <= best_lb) {
            // Repeat until a 2x2 matrix is obtained or the lower bound is too high...

            if (left_branch.get_level() == 0) {
                left_branch.reset_lower_bound();
            }

            // 1. Reduce the matrix in rows and columns.
            cost_t new_cost = left_branch.reduce_cost_matrix();

            // 2. Update the lower bound and check the break condition.
            left_branch.update_lower_bound(new_cost);
            if (left_branch.get_lower_bound() > best_lb) {
                break;
            }

            // 3. Get new vertex and the cost of not choosing it.
            NewVertex new_vertex = NewVertex(left_branch.choose_new_vertex()); // @TODO (KROK 2) //oryginalnie było NewVertex();

            // 4. @TODO Update the path - use append_to_path method.
            left_branch.append_to_path(new_vertex.coordinates);
            // 5. @TODO (KROK 3) Update the cost matrix of the left branch.
            left_branch.update_cost_matrix(new_vertex.coordinates);
            // 6. Update the right branch and push it to the LIFO.
            cost_t new_lower_bound = left_branch.get_lower_bound() + new_vertex.cost;
            tree_lifo.push(create_right_branch_matrix(cm, new_vertex.coordinates,
                                                      new_lower_bound));
        }

        if (left_branch.get_lower_bound() <= best_lb) {
            // If the new solution is at least as good as the previous one,
            // save its lower bound and its path.
            best_lb = left_branch.get_lower_bound();
            path_t new_path = left_branch.get_path();
            solutions.push_back({get_optimal_cost(new_path, cm), new_path});
        }
    }

    for (auto& el: solutions){
        for (auto& element: el.path){
            element += 1;
        }
    }
    return filter_solutions(solutions); // Filter solutions to find only optimal ones.

}
