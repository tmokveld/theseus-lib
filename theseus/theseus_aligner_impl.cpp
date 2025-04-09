#include "theseus/theseus_aligner_impl.h"

TheseusAlignerImpl::TheseusAlignerImpl(Penalties &penalties,
                                       Graph &graph,
                                       bool msa,
                                       bool _score_only) : _orig_penalties(penalties),
                                                          _penalties(penalties),
                                                          _graph(graph),
                                                          _is_msa(msa),
                                                          _is__score_only(_score_only) {
    // TODO: Gap-linear and gap-affine.
    const auto n_scores = std::max({penalties.gapo() + penalties.ins(),
                                   penalties.gapo() + penalties.del(),
                                   penalties.mismatch()}) + 1;

    _scope = std::make_unique<Scope>(n_scores);
    _beyond_scope = std::make_unique<BeyondScope>();
    // constexpr int expected_nvertices = std::max()
    constexpr int expected_nvertices = std::max(1024, /* TODO: */ 0);
    _vertices_data = std::make_unique<VerticesData>(expected_nvertices);
    _scratchpad = std::make_unique<ScratchPad>(-1024, 1024);
}

void new_alignment() {
    // TODO: Avoid recomputing the max_diag if possible.
    int max_diag = 0, v_n;
    for (int l = 0; l < _graph._vertices.size(); ++l) {
      v_n = _graph._vertices[l].value.size();
      max_diag = std::max(max_diag, v_n);
    }
    const int min_diag = -_seq.size();

    if (_scratchpad->max_diag() < max_diag ||
        _scratchpad->min_diag() > min_diag) {
        // TODO: Compute the max and min with a factor.
        _scratchpad = std::make_unique<ScratchPad>(min_diag, max_diag);
    }

    // TODO: Allow for different initial conditions. Now only global alignment.
    // Initial condition Global Alignment
    data_cell init_condition;
    init_condition.offset = 0;
    init_condition.vertex_id = 0;
    init_condition.diagonal = 0;
    init_condition._score_diff = 0;
    // Initial vertex data
    data.M_jumps[0].wf_data.push_back(init_condition);
    data.active_vertices.push_back(0);
    G.vertices[0].pos = 0;
    data.vertices_data.resize(1);
    data.vertices_data[0].M_jumps_positions.resize(_squeue.size());
    data.vertices_data[0].I_jumps_positions.resize(_squeue.size());
    data.vertices_data[0].M_jumps_positions[0].push_back(0);

    
}

TheseusAlignerImpl::Alignment align(std::string_view seq) {
    _scope->new_alignment();
    _beyond_scope->new_alignment();
    _vertices_data->new_alignment();

    // TODO: Set initial conditions

    _score = -1;

    bool end = false;

    // Find the optimal _score and an optimal alignment
    while (!end) {
        // Update _score
        _score = _score + 1;

        // Clear the corresponding waves and metadata from the scope
        _scope->new__score();
        _beyond_scope->new__score();
        _vertices_data->new__score();

        // Compute the values of the new wave
        if (_score == 0) extend_diagonal(&G.vertices[0], data.M_jumps[0].wf_data[0], 0, end_vertex, end, start_pos, data.M_jumps[0].wf_data[0], 0, 'J');
        compute_new_wave(start_pos, end_vertex, end);
    }

}


// Process a given vertex with a given _score
void process_vertex(vertex* curr_v,
                    data_cell &start_pos,
                    int v,
                    int end_vertex,
                    bool &end) {

  // Perform the next operation
  int upper_bound = curr_v->value.size();
  next_I(curr_v, upper_bound, v, end_vertex, end, start_pos);
  _scratchpad.scratch_pad->reset();
  next_D(curr_v, upper_bound, v);
  _scratchpad.scratch_pad->reset();
  next_M(curr_v, upper_bound, v);
  _scratchpad.scratch_pad->reset();

  // Perform the extend operations
  int start_idx, end_idx = _scope._squeue[__score]._m_pos[curr_v->pos];
  if (curr_v->pos == 0) start_idx = 0;
  else start_idx = _scope._squeue[__score]._m_pos[curr_v->pos - 1];
  for (int idx = start_idx; idx < end_idx; ++idx) {
    extend_diagonal(curr_v, data.M[_score].wf_data[idx], v, end_vertex, end, start_pos, data.M[_score].wf_data[idx], idx, 'M');
  }
}


void compute_new_wave(data_cell &start_pos,
                      int end_vertex,
                      bool &end) {

  // Update invalid segments
  _vertices_data->expand();
  _vertices_data->compact();

  // Process all active vertices
  int num_active_vertices = _vertices_data._active_vertices.size(), v;
  for (int l = 0; l < num_active_vertices; ++l) {
    v = _vertices_data._active_vertices[l];
    vertex* curr_v = &_graph._vertices[v];
    process_vertex(curr_v, start_pos, v, __score, end_vertex, end);
  }
}


// Sparsify M data
void sparsify_M_data(
    vertex *curr_v,
    wavefront &dense_wf,
    int offset_increase,
    int shift_factor,
    int start_idx,
    int end_idx,
    int m,
    int upper_bound,
    int vertex_id,
    int new_score_diff) {

  int len = end_idx - start_idx, new_col;
  Cell new_cell;

  // Sparsify the active diagonals
  for (int l = 0; l < len; ++l) {
    new_cell = dense_wf.wf_data[start_idx + l];
    new_cell.diagonal += shift_factor;
    new_cell.offset += offset_increase;
    new_cell._score_diff = new_score_diff;
    new_cell.prev_matrix = 'M';
    new_cell.prev_pos = start_idx + l;
    new_col = new_cell.offset + new_cell.diagonal; // d = j - i -> j = d + i

    // Check validity
    if (new_cell.offset <= m && new_col <= upper_bound) {     // If in bounds
      // Branchless push_back
      auto &cell = _scratchpad.access_alloc(new_cell.diagonal);

      // If better offset
      const bool cmp = cell.offset < new_cell.offset;
      cell = (cmp) ? new_cell :
                     cell;
    }
  }
}


// Sparsify jumps data
void sparsify_jumps_data(
    vertex *curr_v,
    wavefront &dense_wf,
    std::vector<int> &jumps_positions,
    int offset_increase,
    int shift_factor,
    int m,
    int upper_bound,
    int vertex_id,
    int new__score_diff,
    char prev_matrix) {

  int len = jumps_positions.size(), new_col, pos;
  data_cell new_cell;

  // Sparsify the active diagonals
  for (int l = 0; l < len; ++l) {
    pos = jumps_positions[l];
    new_cell = dense_wf.wf_data[pos];
    new_cell.prev_pos = pos;
    new_cell.diagonal += shift_factor;
    new_cell.offset += offset_increase;
    new_cell._score_diff = new__score_diff;
    new_cell.prev_matrix = prev_matrix;
    new_col = new_cell.offset + new_cell.diagonal; // d = j - i -> j = d + i

    // Check validity
    if (new_cell.offset <= m && new_col <= upper_bound) {     // If in bounds
      // Branchless push_back
      auto &cell = scratch_pad.access_alloc(new_cell.diagonal);

      // If better offset
      const bool cmp = cell.offset < new_cell.offset;
      cell = (cmp) ? new_cell :
                     cell;
    }
  }
}


// Sparsify indel
void sparsify_indel_data(
    vertex *curr_v,
    wavefront &dense_wf,
    ScratchPad &scratch_pad,
    int offset_increase,
    int shift_factor,
    int start_idx,
    int end_idx,
    int m,
    int upper_bound,
    int vertex_id,
    int added_score_diff) {

  int len = end_idx - start_idx, new_col;
  data_cell new_cell;

  // Sparsify the active diagonals
  for (int l = 0; l < len; ++l) {
    // Vertex_id and previous matrix are the same as before
    new_cell = dense_wf.wf_data[start_idx + l];
    new_cell.diagonal += shift_factor;
    new_cell.offset += offset_increase;
    new_cell._score_diff += added__score_diff;
    new_col = new_cell.offset + new_cell.diagonal; // d = j - i -> j = d + i

    // Check validity
    if (new_cell.offset <= m && new_col <= upper_bound) {     // If in bounds
      // Branchless push_back
      auto &cell = scratch_pad.access_alloc(new_cell.diagonal);

      // If better offset
      const bool cmp = cell.offset < new_cell.offset;
      cell = (cmp) ? new_cell :
                     cell;
    }
  }
}


// Compute next I matrix
void next_I(vertex* curr_v,
            int upper_bound,
            int v,
            int end_vertex,
            bool &end,
            data_cell &start_pos) {

    // Sparsify data (put it in the scratch pad)
    int pos_prev_M = _score - (_penalties.gapo() + _penalties.ins()), pos_prev_I = _score - penalties.ins(), start_idx, end_idx;

    // Come from an Insertion
    if (pos_prev_I >= 0 && _scope._squeue[pos_prev_I]._i_pos.size() > curr_v->pos)  {
      if (curr_v->pos > 0) start_idx = _scope._squeue[pos_prev_I]._i_pos[curr_v->pos - 1];
      else start_idx = 0;
      end_idx = _scope._squeue[pos_prev_I]._i_pos[curr_v->pos];
      sparsify_indel_data(curr_v, _scope._squeue[pos_prev_I]._i_wf, 0, 1, start_idx, end_idx, _seq.size(), upper_bound, v, _penalties.ins());  // Sparsify I data
      sparsify_jumps_data(curr_v, _beyond_scope[pos_prev_I]._i_jumps, _vertices_data._jumps_pos[curr_v->pos]._i_jumps_positions[pos_prev_I], 0, 1, _seq.size(), upper_bound, v, _penalties.ins(), 'I');
    }

    // Come from M
    if (pos_prev_M >= 0 && _scope._squeue[pos_prev_M]._m_pos.size() > curr_v->pos) {
      if (curr_v->pos > 0) start_idx = data.M_positions[pos_prev_M][curr_v->pos - 1];
      else start_idx = 0;
      end_idx = data.M_positions[pos_prev_M][curr_v->pos];
      sparsify_M_data(curr_v, _beyond_scope[pos_prev_M]._m_wf, 0, 1, start_idx, end_idx,  _seq.size(), upper_bound, v, _penalties.gapo() + _penalties.ins());  // Sparsify M data
      sparsify_jumps_data(curr_v, _beyond_scope[pos_prev_M]._m_jumps, _vertices_data._jumps_pos[curr_v->pos]._m_jumps_positions[pos_prev_M], 0, 1, _seq.size(), upper_bound, v, _penalties.gapo() + _penalties.ins(), 'J');
    }

    // Densify data (store it in the big wavefront)
    int len = data.scratch_pad->nactive_diags(), diag_pos, diag;
    for (auto diag : _scratchpad->active_diags()) {
      _vertices_data->valid_diagonal<Cell::Matrix::I>(curr_v->pos, diag);
      _scope._squeue[_score]._i_wf.push_back((*data.scratch_pad)[diag]);     // Store data_cell
    }
    int new_end = _scope._squeue[_score]._i_wf.size();
    _scope._squeue[_score]._i_pos.push_back(new_end);

    // Check, store and invalidate new I jumps
    if (curr_v->pos == 0) start_idx = 0;
    else start_idx = _scope._squeue[_score]._i_pos[curr_v->pos - 1];
    if (curr_v->out_vertices.size() > 0) {
      check_and_store_jumps(curr_v, _scope._squeue[_score]._i_wf, start_idx, new_end, v, end_vertex, end, start_pos);
    }
}


// Compute next D matrix
void next_D(vertex* curr_v,
            int upper_bound,
            int v) {

    // Sparsify data (put it in the scratch pad)
    int pos_prev_M = _score - (_penalties.gapo() + _penalties.ins()), pos_prev_D = _score - penalties.ins(), start_idx, end_idx;

    // Come from a Deletion
    if (pos_prev_D >= 0 && _scope._squeue[pos_prev_D]._d_pos.size() > curr_v->pos)  {
      if (curr_v->pos > 0) start_idx = _scope._squeue[pos_prev_D]._d_pos[curr_v->pos - 1];
      else start_idx = 0;
      end_idx = _scope._squeue[pos_prev_D]._d_pos[curr_v->pos];
      sparsify_indel_data(curr_v, _scope._squeue[pos_prev_D]._d_wf, 1, -1, start_idx, end_idx, _seq.size(), upper_bound, v, _penalties.del());  // Sparsify D data
    }

    // Come from M
    if (pos_prev_M >= 0 && _scope._squeue[pos_prev_M]._m_pos.size() > curr_v->pos) {
      if (curr_v->pos > 0) start_idx = data.M_positions[pos_prev_M][curr_v->pos - 1];
      else start_idx = 0;
      end_idx = data.M_positions[pos_prev_M][curr_v->pos];
      sparsify_M_data(curr_v, _beyond_scope[pos_prev_M]._m_wf, 1, -1, start_idx, end_idx,  _seq.size(), upper_bound, v, _penalties.gapo() + _penalties.del());  // Sparsify M data
      sparsify_jumps_data(curr_v, _beyond_scope[pos_prev_M]._m_jumps, _vertices_data._jumps_pos[curr_v->pos]._m_jumps_positions[pos_prev_M], 1, -1, _seq.size(), upper_bound, v, _penalties.gapo() + _penalties.del(), 'J');
    }

    // Densify data (store it in the big wavefront)
    int len = data.scratch_pad->nactive_diags(), diag_pos, diag;
    for (auto diag : _scratchpad->active_diags()) {
      _vertices_data->valid_diagonal<Cell::Matrix::D>(curr_v->pos, diag);
      _scope._squeue[_score]._d_wf.push_back((*data.scratch_pad)[diag]);     // Store data_cell
    }
    int new_end = _scope._squeue[_score]._d_wf.size();
    _scope._squeue[_score]._d_pos.push_back(new_end);
}


// Compute next M matrix
void next_M(
    vertex* curr_v,
    int upper_bound,
    int v) {

  // Sparsify data (put it in the scratch pad)
  int pos_prev_M = _score - _penalties.subs(), pos_prev_D = _score, pos_prev_I = _score, start_idx, end_idx;

  // Come from a Deletion
  if (_scope._squeue[pos_prev_D]._d_pos.size() > curr_v->pos)  {
    if (curr_v->pos > 0) start_idx = _scope._squeue[pos_prev_D]._d_pos[curr_v->pos - 1];
    else start_idx = 0;
    end_idx = _scope._squeue[pos_prev_D]._d_pos[curr_v->pos];
    sparsify_indel_data(curr_v, _scope._squeue[pos_prev_D]._d_wf, 0, 0, start_idx, end_idx, _seq.size(), upper_bound, v, 0);  // Sparsify D data
  }

  // Come from an Insertion
  if (_scope._squeue[pos_prev_I]._i_pos.size() > curr_v->pos)  {
    if (curr_v->pos > 0) start_idx = _scope._squeue[pos_prev_I]._i_pos[curr_v->pos - 1];
    else start_idx = 0;
    end_idx = _scope._squeue[pos_prev_I]._i_pos[curr_v->pos];
    sparsify_indel_data(curr_v, _scope._squeue[pos_prev_I]._d_wf, 0, 0, start_idx, end_idx, _seq.size(), upper_bound, v, 0);  // Sparsify I data
  }

  // Come from M
  if (pos_prev_M >= 0 && _scope._squeue[pos_prev_M]._m_pos.size() > curr_v->pos) {
    if (curr_v->pos > 0) start_idx = data.M_positions[pos_prev_M][curr_v->pos - 1];
    else start_idx = 0;
    end_idx = data.M_positions[pos_prev_M][curr_v->pos];
    sparsify_M_data(curr_v, _beyond_scope[pos_prev_M]._m_wf, 1, 0, start_idx, end_idx,  _seq.size(), upper_bound, v, _penalties.subs());  // Sparsify M data
    sparsify_jumps_data(curr_v, _beyond_scope[pos_prev_M]._m_jumps, _vertices_data._jumps_pos[curr_v->pos]._m_jumps_positions[pos_prev_M], 1, 0, _seq.size(), upper_bound, v, _penalties.subs(), 'J');
  }

  // Densify data (store it in the big wavefront)
  int len = data.scratch_pad->nactive_diags(), diag_pos, diag;
  for (auto diag : _scratchpad->active_diags()) {
    _vertices_data->valid_diagonal<Cell::Matrix::M>(curr_v->pos, diag);
    _beyond_scope[_score]._m_wf.push_back((*data.scratch_pad)[diag]);     // Store data_cell
  }
  int new_end =_beyond_scope[_score]._m_wf.size();
  _scope._squeue[_score]._m_pos.push_back(new_end);
}



// Store the jump in neighbours
void store_M_jump(vertex* curr_v,
                  Cell &prev_cell,
                  int end_vertex,
                  bool &end,
                  Cell &start_pos,
                  int prev_pos,
                  char prev_matrix,
                  int _score_diff) {

  // Invalidate the jumping diagonal
  invalidate_M_jump(prev_cell.diagonal, data.vertices_data[curr_v->pos]);

  int num_out_v = curr_v->out_vertices.size();
  Cell new_cell = prev_cell;
  new_cell.diagonal = -new_cell.offset;
  new_cell.prev_matrix = prev_matrix;
  new_cell.prev_pos = prev_pos;
  new_cell._score_diff = _score_diff;

  for (int l = 0; l < num_out_v; ++l) {
    new_cell.vertex_id = curr_v->out_vertices[l];
    vertex* new_v = &G.vertices[curr_v->out_vertices[l]];
    activate_vertex(new_cell.vertex_id);

    // Store jump and metadata
    _vertices_data->valid_diagonal<Cell::Matrix::M>(new_cell.vertex_id, new_cell.diagonal); // Extend only if it has not yet been visited
    if (valid_diagonal_M(new_cell.diagonal, data.vertices_data[new_v->pos])) { // Extend only if it has not yet been visited
      int pos_new_cell = data.M_jumps[_score].wf_data.size();
      data.M_jumps[_score].wf_data.push_back(new_cell);
      data.vertices_data[new_v->pos].M_jumps_positions[_score%_squeue.size()].push_back(pos_new_cell);
      extend_diagonal(G, data, new_v, seq, data.M_jumps[_score].wf_data[pos_new_cell], _score, new_cell.vertex_id, end_vertex, end, start_pos, data.M_jumps[_score].wf_data[pos_new_cell], pos_new_cell, 'J');
    }
  }
}


// Store the jump in neighbours
void store_I_jump(vertex* curr_v,
                  data_cell& prev_cell,
                  int prev_pos,
                  char prev_matrix) {

  // Invalidate the jumping diagonal
  invalidate_I_jump(prev_cell.diagonal, data.vertices_data[curr_v->pos]);

  wavefront new_wave;
  int len = curr_v->out_vertices.size();
  int _squeue.size() = INDEL_OPEN + INDEL_EXTEND + 1;
  data_cell new_cell = prev_cell;
  new_cell.diagonal = -new_cell.offset;
  new_cell.prev_matrix = prev_matrix;
  new_cell.prev_pos = prev_pos;
  for (int l = 0; l < len; ++l) {
    new_cell.vertex_id = curr_v->out_vertices[l];
    vertex* new_v = &G.vertices[curr_v->out_vertices[l]];

    if (new_v->pos == -INT_MAX) {
      new_v->pos = data.active_vertices.size();
      data.active_vertices.push_back(new_cell.vertex_id);
      data.vertices_data.resize(new_v->pos + 1);
      data.vertices_data[new_v->pos].I_jumps_positions.resize(_squeue.size());
      data.vertices_data[new_v->pos].M_jumps_positions.resize(_squeue.size());
    }

    // Store jump and metadata
    if (valid_diagonal_I(new_cell.diagonal, data.vertices_data[new_v->pos])) { // Extend only if it has not yet been visited
      data.vertices_data[new_v->pos].I_jumps_positions[_score%_squeue.size()].push_back(data.I_jumps[_score].wf_data.size());
      data.I_jumps[_score].wf_data.push_back(new_cell);
    }
  }
}


// Check and store I jumps (that is, those diagonals that have reached the last column of a vertex)
void check_and_store_jumps(vertex* curr_v,
                          std::vector<Cell> &curr_wavefront,
                          int start_idx,
                          int end_idx,
                          int v,
                          int end_vertex,
                          bool &end,
                          data_cell &start_pos) {

  int len = end_idx - start_idx, diag, offset, curr_j, n = curr_v->value.size(), prev_pos;
  char prev_matrix;

  for (int l = 0; l < len; ++l) {
    diag = curr_wavefront[start_idx + l].diagonal;
    offset = curr_wavefront[start_idx + l].offset;
    curr_j = diag + offset;
    if (curr_j == n && offset <= data.m) {
      prev_matrix = curr_wavefront.[start_idx + l].prev_matrix;
      prev_pos = curr_wavefront.[start_idx + l].prev_pos;
      store_M_jump(curr_v, curr_wavefront.[start_idx + l], end_vertex, end, start_pos, prev_pos, prev_matrix, curr_wavefront[start_idx + l]._score_diff);
      store_I_jump(curr_v, curr_wavefront.[start_idx + l], prev_pos, prev_matrix);
    }
  }
}


// Compute the Longest Common Prefix between two given sequences
void LCP(std::string &seq_1,
        std::string &seq_2,
        int len_seq_1,
        int len_seq_2,
        int &offset, // pointer to the offset (row value) at the current diagonal
        int &j) {

  // Find LCP
  while (offset < len_seq_1 && j < len_seq_2 && seq_1[offset] == seq_2[j]) {
    offset = offset + 1; // Update the f.r. of this diagonal
    j = j + 1;
  }
}


// Check end condition (Global alignment)
void check_end_condition(
    align_data &data,
    data_cell curr_data, // Offset and prev_index
    int j,
    int v,
    int end_vertex,
    bool &end,
    data_cell &start_pos) {

  int j_end = 0; // The last node is empty
  if (curr_data.offset == data.m && v == end_vertex && j == j_end) { // End condition global alignment
    end = true;
    start_pos = curr_data;
  }
}


// Extend a particular diagonal
void extend_diagonal(
    graph &G,
    align_data &data,
    vertex *curr_v,
    std::string &seq,
    data_cell &curr_cell,
    int score,
    int v,
    int end_vertex,
    bool &end,
    data_cell &start_pos,
    data_cell &prev_cell,
    int prev_pos,
    char prev_matrix) {

  int n = curr_v->value.size();

  // Longest Common prefix
  int diag = curr_cell.diagonal;
  int j = diag + curr_cell.offset;
  LCP(seq, curr_v->value, data.m, n, curr_cell.offset, j); // Find Longest Common Prefix

  // End condition
  check_end_condition(data, curr_cell, j, v, end_vertex, end, start_pos); // Check end condition

  // Check jump
  if (j == n && curr_cell.offset <= data.m && curr_v->out_vertices.size() > 0) {
    store_M_jump(G, data, curr_v, prev_cell, seq, score, end_vertex, end, start_pos, prev_pos, prev_matrix, 0);
  }
}



// Check whether the alignment is consistent or not
bool consistent_alignment(
    backtrack_seq& back,
    std::string& new_seq,
    int start_v,
    int end_v) {

  // Iterators over the two strings
  int i = 0; // Recovered sequence
  int j = 0; // Modified sequence

  // Check if it starts at start_v and ends at end_v
  if (back.path[0] != start_v || back.path[back.path.size() - 1] != end_v) {
    std::cout << "Starting at " << back.path[0] << " and ending at " << back.path[back.path.size() - 1] << std::endl;
  }

  for (int k = 0; k < back.edit_op.size(); ++k) {
    if (back.edit_op[k] == 'I') { // Insertion
      j = j + 1;
    }
    else if (back.edit_op[k] == 'D') { // Deletion
      i = i + 1;
    }
    else {
      if (back.edit_op[k] == 'M') { // Match
        if (back.rec_seq[j] != new_seq[i]) {
          std::cout << "The recovered sequence is not consistent in a MATCH. K = " << k << std::endl;
          break;
        }
      }
      else if (back.edit_op[k] == 'X') { // Mismatch
        if (back.rec_seq[j] == new_seq[i]) {
          std::cout << "The recovered sequence is not consistent in a MISMATCH. K = " << k << std::endl;
          break;
        }
      }
      else { // Special match
        if (back.rec_seq[j] & new_seq[i] != new_seq[i]) {
          std::cout << "The recovered sequence is not consistent in a SPECIAL MATCH. K = " << k << std::endl;
          break;
        }
      }

      i = i + 1;
      j = j + 1;
    }
  }

  if (j == back.rec_seq.size() && i == new_seq.size()) {
    return true;
  }
  else {
    return false;
  }
}


// Add matches to our backtracking vector
void add_matches(
    backtrack_seq &back,
    std::string &seq,
    std::string &seq_graph,
    int start_matches,
    int end_matches) {

  int size = end_matches - start_matches;
  for (int k = 0; k < size; ++k) {
    char character_to_add_seq = seq[end_matches - k - 1];
    back.edit_op.push_back('M');
    back.rec_seq.push_back(character_to_add_seq);
  }
}


// Add a mismatch to our backtracking vector
void add_mismatch(
    graph &G,
    backtrack_seq &back,
    std::string &seq_graph,
    data_cell curr_pos) {

  int j = curr_pos.offset + curr_pos.diagonal - 1;
  char character_to_add = G.vertices[curr_pos.vertex_id].value[j];     // Suposem que la posició
  back.edit_op.push_back('X');                                         // està ben guardada
  back.rec_seq.push_back(character_to_add);
}


// Add an insertion to our backtracking vector
void add_insertion(
    graph &G,
    backtrack_seq &back,
    data_cell &curr_pos) {

  int j = curr_pos.offset + curr_pos.diagonal - 1;
  char character_to_add = G.vertices[curr_pos.vertex_id].value[j];
  back.edit_op.push_back('I');
  back.rec_seq.push_back(character_to_add);
}


// Add a deletion to our backtracking vector
void add_deletion(backtrack_seq &back) {

  back.edit_op.push_back('D');
}


void one_backtrace_step(
    graph &G,
    align_data &data,
    data_cell &curr_cell,
    backtrack_seq &back,
    std::string &seq,
    vertex *curr_v,
    int &score) {

  // Get previous cell
  score -= curr_cell.score_diff;
  data_cell prev_cell;
  if (curr_cell.prev_matrix == 'M') prev_cell = data.M[score].wf_data[curr_cell.prev_pos];
  else if (curr_cell.prev_matrix == 'J') prev_cell = data.M_jumps[score].wf_data[curr_cell.prev_pos];
  else prev_cell = data.I_jumps[score].wf_data[curr_cell.prev_pos];

  // Inside the same vertex or jump
  int num_indels;
  if (curr_cell.vertex_id == prev_cell.vertex_id) { // Still in the same vertex
    if (curr_cell.diagonal == prev_cell.diagonal) {                                               // Mismatch
      if (curr_cell.offset > prev_cell.offset) {    // Consider 0 length vertices
        add_matches(back, seq, curr_v->value, prev_cell.offset + 1, curr_cell.offset);
        add_mismatch(G, back, curr_v->value, curr_cell);
      }
    }
    else { // Indel
      if (curr_cell.diagonal < prev_cell.diagonal) {                                              // Deletion
        num_indels = prev_cell.diagonal - curr_cell.diagonal;
        add_matches(back, seq, curr_v->value, prev_cell.offset + num_indels, curr_cell.offset);
        for (int l = 0; l < num_indels; ++l) add_deletion(back);
      }
      else {                                                                                      // Insertion
        num_indels = curr_cell.diagonal - prev_cell.diagonal;
        add_matches(back, seq, curr_v->value, prev_cell.offset, curr_cell.offset);
        for (int l = 0; l < num_indels; ++l) add_insertion(G, back, curr_cell);
      }
    }
  }
  else {                                            // Jump
    add_matches(back, seq, curr_v->value, prev_cell.offset, curr_cell.offset);                    // Add the necessary matches
    back.path.push_back(prev_cell.vertex_id); // Add the new vertex to the path
    int col_in_prev_v = prev_cell.diagonal + prev_cell.offset;
    int num_insertions = G.vertices[prev_cell.vertex_id].value.size() - col_in_prev_v;
    for (int l = 0; l < num_insertions; ++l) add_insertion(G, back, curr_cell);                   // Add the necessary insertions
  }

  curr_cell = prev_cell;
}


// Main function of the backtracking process
void backtracking_gap_affine(
    graph &G,
    align_data &data,
    std::string &seq,
    backtrack_seq &back,
    data_cell &start_pos,
    int initial_vertex,
    int score) {

  data_cell curr_pos = start_pos;
  vertex *curr_v = &G.vertices[start_pos.vertex_id];
  back.path.push_back(curr_pos.vertex_id); // Guardo l'últim node?
  while (curr_pos.vertex_id != initial_vertex) {
    curr_v = &G.vertices[curr_pos.vertex_id];
    one_backtrace_step(G, data, curr_pos, back, seq, curr_v, score);
  }

  int remaining_deletions = curr_pos.offset;
  for (int l = 0; l < remaining_deletions; ++l) { // If the alignment started with some deletions, add them
    back.edit_op.push_back('D');
  }

  std::reverse(back.edit_op.begin(), back.edit_op.end());
  std::reverse(back.rec_seq.begin(), back.rec_seq.end());
  std::reverse(back.path.begin(), back.path.end());
}