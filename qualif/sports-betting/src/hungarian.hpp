/********************************************************************
 ********************************************************************
 ** C++ class implementation of the Hungarian algorithm by David Schwarz, 2012
 **
 **
 ** O(n^3) implementation derived from libhungarian by Cyrill Stachniss, 2004
 **
 **
 ** Solving the Minimum Assignment Problem using the
 ** Hungarian Method.
 **
 ** ** This file may be freely copied and distributed! **
 **
 **
 ** This file is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied
 ** warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 ** PURPOSE.
 **
 ********************************************************************
 ********************************************************************/

#include <algorithm>
#include <iostream>
#include <vector>
#include <limits>
#include <stdio.h>

#ifndef HUNGARIAN_H
#define HUNGARIAN_H

using std::vector;

typedef enum {
    HUNGARIAN_MODE_MINIMIZE_COST,
    HUNGARIAN_MODE_MAXIMIZE_UTIL,
} MODE;

typedef enum {
    HUNGARIAN_NOT_ASSIGNED,
    HUNGARIAN_ASSIGNED,
} ASSIGN;

template <typename Num> class Hungarian {

  public:
    /** This method initialize the hungarian_problem structure and init
     *  the  cost matrices (missing lines or columns are filled with 0).
     *  It returns the size of the quadratic(!) assignment matrix. **/

    Hungarian();
    Hungarian(const vector<vector<Num>> &, int, int, MODE);

    int init(const vector<vector<Num>> &input_matrix, int rows, int cols,
             MODE mode);

    /** This method computes the optimal assignment. **/
    bool solve();

    /** Accessor for the cost **/
    Num cost() const;

    /** Reference accessor for assignment **/
    const vector<vector<int>> &assignment() const;

    /** Reference accessor for cost **/
    const vector<vector<Num>> &cost_matrix() const;

    /** Print the computed optimal assignment. **/
    void print_assignment();

    /** Print the cost matrix. **/
    void print_cost();

    /** Print cost matrix and assignment matrix. **/
    void print_status();

  protected:
    bool check_solution(const vector<Num> &row_dec, const vector<Num> &col_inc,
                        const vector<int> &col_vertex);
    bool assign_solution(const vector<Num> &row_dec, const vector<Num> &col_inc,
                         const vector<int> &col_vertex);

  private:
    Num m_cost;
    int m_rows;
    int m_cols;
    vector<vector<Num>> m_costmatrix;
    vector<vector<int>> m_assignment;
};

#define verbose (0)

template <typename Num> inline Hungarian<Num>::Hungarian() {
    // much ado about nothing
    m_rows = 1;
    m_cols = 1;

    m_costmatrix.resize(m_rows, vector<Num>(m_cols, 0));
    m_assignment.resize(m_rows, vector<Num>(m_cols, 0));
}

template <typename Num>
inline Hungarian<Num>::Hungarian(const vector<vector<Num>> &input_matrix,
                                 int rows, int cols, MODE mode) {
    int i, j, org_cols, org_rows;
    Num max_cost = Num{0};

    org_cols = cols;
    org_rows = rows;

    // is the matrix square?
    // if no, expand with 0-cols / 0-cols

    if (rows != cols) {
        rows = std::max(cols, rows);
        cols = rows;
    }

    m_rows = rows;
    m_cols = cols;

    m_costmatrix.resize(rows, vector<Num>(cols, Num{0}));
    m_assignment.resize(rows, vector<int>(cols, 0));

    for (i = 0; i < m_rows; i++) {
        for (j = 0; j < m_cols; j++) {
            m_costmatrix[i][j] =
                (i < org_rows && j < org_cols) ? input_matrix[i][j] : Num{0};
            m_assignment[i][j] = 0;

            if (max_cost < m_costmatrix[i][j]) {
                max_cost = m_costmatrix[i][j];
            }
        }
    }

    if (mode == HUNGARIAN_MODE_MAXIMIZE_UTIL) {
        for (i = 0; i < m_rows; i++) {
            for (j = 0; j < m_cols; j++) {
                m_costmatrix[i][j] = max_cost - m_costmatrix[i][j];
            }
        }
    } else if (mode == HUNGARIAN_MODE_MINIMIZE_COST) {
        // nothing to do
    } else
        fprintf(stderr, "%s: unknown mode. Mode was set to "
                        "HUNGARIAN_MODE_MINIMIZE_COST !\n",
                __FUNCTION__);
}

template <typename Num>
inline void hungarian_print_matrix(const vector<vector<Num>> &C, int rows,
                                   int cols) {
    int i, j;
    std::cerr << std::endl;
    for (i = 0; i < rows; i++) {
        std::cerr << " [";
        for (j = 0; j < cols; j++) {
            std::cerr << C[i][j] << "\t";
        }
        std::cerr << "]" << std::endl;;
    }
    std::cerr << std::endl;
}

template <typename Num> inline void Hungarian<Num>::print_assignment() {
    hungarian_print_matrix(m_assignment, m_rows, m_cols);
}

template <typename Num> inline void Hungarian<Num>::print_cost() {
    hungarian_print_matrix(m_costmatrix, m_rows, m_cols);
}

template <typename Num> inline void Hungarian<Num>::print_status() {

    std::cerr << "cost:" << std::endl;
    print_cost();

    std::cerr << "assignment:" << std::endl;
    print_assignment();
}

template <typename Num>
inline int Hungarian<Num>::init(const vector<vector<Num>> &input_matrix,
                                int rows, int cols, MODE mode) {

    int i, j, org_cols, org_rows;
    Num max_cost = Num{0};

    org_cols = cols;
    org_rows = rows;

    // is the number of cols  not equal to number of rows ?
    // if yes, expand with 0-cols / 0-cols
    rows = std::max(cols, rows);
    cols = rows;

    m_rows = rows;
    m_cols = cols;

    m_costmatrix.resize(rows, vector<Num>(cols, Num{0}));
    m_assignment.resize(rows, vector<Num>(cols, Num{0}));

    for (i = 0; i < m_rows; i++) {
        for (j = 0; j < m_cols; j++) {
            m_costmatrix[i][j] =
                (i < org_rows && j < org_cols) ? input_matrix[i][j] : Num{0};
            m_assignment[i][j] = Num{0};

            if (max_cost < m_costmatrix[i][j])
                max_cost = m_costmatrix[i][j];
        }
    }

    if (mode == HUNGARIAN_MODE_MAXIMIZE_UTIL) {
        for (i = 0; i < m_rows; i++) {
            for (j = 0; j < m_cols; j++) {
                m_costmatrix[i][j] = max_cost - m_costmatrix[i][j];
            }
        }
    } else if (mode == HUNGARIAN_MODE_MINIMIZE_COST) {
        // nothing to do
    } else
        fprintf(stderr, "%s: unknown mode. Mode was set to "
                        "HUNGARIAN_MODE_MINIMIZE_COST !\n",
                __FUNCTION__);

    return rows;
}

template <typename Num>
inline bool Hungarian<Num>::check_solution(const vector<Num> &row_dec,
                                           const vector<Num> &col_inc,
                                           const vector<int> &col_vertex) {
    int k, l, m, n;

    m = m_rows;
    n = m_cols;
    // Begin doublecheck the solution 23
    for (k = 0; k < m; k++)
        for (l = 0; l < n; l++)
            if (m_costmatrix[k][l] < row_dec[k] - col_inc[l])
                return false;

    for (k = 0; k < m; k++) {
        l = col_vertex[k];
        if (l < 0 || m_costmatrix[k][l] != row_dec[k] - col_inc[l])
            return false;
    }
    k = 0;

    for (l = 0; l < n; l++) {
        if (col_inc[l]) {
            k++;
        }
    }
    if (k > m) {
        return false;
    }

    // everything checks out, then
    return true;
    // End doublecheck the solution 23
}
template <typename Num>
inline bool Hungarian<Num>::assign_solution(const vector<Num> &row_dec,
                                            const vector<Num> &col_inc,
                                            const vector<int> &col_vertex) {
    // End Hungarian algorithm 18
    int i, j, k, l, m, n;

    m = m_rows;
    n = m_cols;

    for (i = 0; i < m; ++i) {
        m_assignment[i][col_vertex[i]] = HUNGARIAN_ASSIGNED;
        /*TRACE("%d - %d\n", i, col_vertex[i]);*/
    }
    for (k = 0; k < m; ++k) {
        for (l = 0; l < n; ++l) {
            /*TRACE("%d ",m_costmatrix[k][l]-row_dec[k]+col_inc[l]);*/
            m_costmatrix[k][l] = m_costmatrix[k][l] - row_dec[k] + col_inc[l];
        }
        /*TRACE("\n");*/
    }
    for (i = 0; i < m; i++) {
        m_cost += row_dec[i];
    }
    for (i = 0; i < n; i++) {
        m_cost -= col_inc[i];
    }
    if (verbose)
        fprintf(stderr, "Cost is %d\n", m_cost);

    return true;
}

template <typename Num> inline bool Hungarian<Num>::solve() {
    int i, j, m, n, k, l, t, q, unmatched;
    Num s;

    m = m_rows;
    n = m_cols;

    Num INF = Num{std::numeric_limits<int>::max()};

    // vertex alternating paths,
    vector<int> col_vertex(m), row_vertex(n), unchosen_row(m), parent_row(n),
        slack_row(m);
    vector<Num> slack(n), row_dec(m), col_inc(n);

    Num cost = Num{0};

    for (i = 0; i < m_rows; i++) {
        col_vertex[i] = 0;
        unchosen_row[i] = 0;
        row_dec[i] = Num{0};
        slack_row[i] = 0;
    }

    for (j = 0; j < m_cols; j++) {
        row_vertex[j] = 0;
        parent_row[j] = 0;
        col_inc[j] = Num{0};
        slack[j] = Num{0};
    }

    // Double check assignment matrix is 0
    m_assignment.assign(m, vector<int>(n, HUNGARIAN_NOT_ASSIGNED));

    // Begin subtract column minima in order to start with lots of zeroes 12
    if (verbose) {
        fprintf(stderr, "Using heuristic\n");
    }

    for (l = 0; l < n; l++) {
        s = m_costmatrix[0][l];

        for (k = 1; k < m; k++) {
            if (m_costmatrix[k][l] < s) {
                s = m_costmatrix[k][l];
            }
            cost += s;
        }

        if (s != Num{0}) {
            for (k = 0; k < m; k++) {
                m_costmatrix[k][l] -= s;
            }
        }

        // pre-initialize state 16
        row_vertex[l] = -1;
        parent_row[l] = -1;
        col_inc[l] = Num{0};
        slack[l] = INF;
    }
    // End subtract column minima in order to start with lots of zeroes 12

    // Begin initial state 16
    t = 0;

    for (k = 0; k < m; k++) {
        bool row_done = false;
        s = m_costmatrix[k][0];

        for (l = 0; l < n; l++) {

            if (l > 0) {
                if (m_costmatrix[k][l] < s) {
                    s = m_costmatrix[k][l];
                }
                row_dec[k] = s;
            }

            if (s == m_costmatrix[k][l] && row_vertex[l] < 0) {
                col_vertex[k] = l;
                row_vertex[l] = k;

                if (verbose) {
                    fprintf(stderr, "matching col %d==row %d\n", l, k);
                }
                row_done = true;
                break;
            }
        }

        if (!row_done) {
            col_vertex[k] = -1;

            if (verbose) {
                fprintf(stderr, "node %d: unmatched row %d\n", t, k);
            }

            unchosen_row[t++] = k;
        }
    }
    // End initial state 16

    bool checked = false;

    // Begin Hungarian algorithm 18

    // is matching already complete?
    if (t == 0) {
        checked = check_solution(row_dec, col_inc, col_vertex);
        if (checked) {
            // finish assignment, wrap up and done.
            bool assign = assign_solution(row_dec, col_inc, col_vertex);
            return true;
        } else {
            if (verbose) {
                fprintf(stderr, "Could not solve. Error.\n");
            }
            return false;
        }
    }

    unmatched = t;

    while (1) {
        if (verbose) {
            fprintf(stderr, "Matched %d rows.\n", m - t);
        }
        q = 0;
        bool try_matching;
        while (1) {
            while (q < t) {
                // Begin explore node q of the forest 19

                k = unchosen_row[q];
                s = row_dec[k];
                for (l = 0; l < n; l++) {
                    if (slack[l]) {
                        Num del;
                        del = m_costmatrix[k][l] - s + col_inc[l];
                        if (del < slack[l]) {
                            if (del == Num{0}) {
                                if (row_vertex[l] < 0) {
                                    goto breakthru;
                                }
                                slack[l] = Num{0};
                                parent_row[l] = k;
                                if (verbose)
                                    fprintf(stderr,
                                            "node %d: row %d==col %d--row %d\n",
                                            t, row_vertex[l], l, k);
                                unchosen_row[t++] = row_vertex[l];
                            } else {
                                slack[l] = del;
                                slack_row[l] = k;
                            }
                        }
                    }
                }
                // End explore node q of the forest 19
                q++;
            }

            // Begin introduce a new zero into the matrix 21
            s = INF;
            for (l = 0; l < n; l++) {
                if (slack[l] && slack[l] < s) {
                    s = slack[l];
                }
            }
            for (q = 0; q < t; q++) {
                row_dec[unchosen_row[q]] += s;
            }
            for (l = 0; l < n; l++) {
                // check slack
                if (slack[l]) {
                    slack[l] -= s;
                    if (slack[l] == Num{0}) {
                        // Begin look at a new zero 22
                        k = slack_row[l];
                        if (verbose) {
                            fprintf(stderr, "Decreasing uncovered elements by "
                                            "%d produces zero at [%d,%d]\n",
                                    s, k, l);
                        }
                        if (row_vertex[l] < 0) {
                            for (j = l + 1; j < n; j++)
                                if (slack[j] == Num{0}) {
                                    col_inc[j] += s;
                                }

                            goto breakthru;
                        } else {
                            parent_row[l] = k;
                            if (verbose) {
                                fprintf(stderr,
                                        "node %d: row %d==col %d--row %d\n", t,
                                        row_vertex[l], l, k);
                                unchosen_row[t++] = row_vertex[l];
                            }
                        }
                        // End look at a new zero 22
                    }
                } else {
                    col_inc[l] += s;
                }
            }
            // End introduce a new zero into the matrix 21
        }

    breakthru:
        // Begin update the matching 20
        if (verbose) {
            fprintf(stderr, "Breakthrough at node %d of %d!\n", q, t);
        }
        while (1) {
            j = col_vertex[k];
            col_vertex[k] = l;
            row_vertex[l] = k;
            if (verbose) {
                fprintf(stderr, "rematching col %d==row %d\n", l, k);
            }
            if (j < 0) {
                break;
            }
            k = parent_row[j];
            l = j;
        }
        // End update the matching 20
        if (--unmatched == 0) {
            checked = check_solution(row_dec, col_inc, col_vertex);
            if (checked) {
                // finish assignment, wrap up and done.
                bool assign = assign_solution(row_dec, col_inc, col_vertex);
                return true;
            } else {
                if (verbose) {
                    fprintf(stderr, "Could not solve. Error.\n");
                }
                return false;
            }
        }

        // Begin get ready for another stage 17
        t = 0;
        for (l = 0; l < n; l++) {
            parent_row[l] = -1;
            slack[l] = INF;
        }
        for (k = 0; k < m; k++) {
            if (col_vertex[k] < 0) {
                if (verbose)
                    fprintf(stderr, "node %d: unmatched row %d\n", t, k);
                unchosen_row[t++] = k;
            }
        }
        // End get ready for another stage 17
    } // back to while loop
}

// ACCESSORS

template <typename Num> inline Num Hungarian<Num>::cost() const {
    return m_cost;
}

template <typename Num>
inline const vector<vector<int>> &Hungarian<Num>::assignment() const {
    return m_assignment;
}

template <typename Num>
inline const vector<vector<Num>> &Hungarian<Num>::cost_matrix() const {
    return m_costmatrix;
}
#endif
