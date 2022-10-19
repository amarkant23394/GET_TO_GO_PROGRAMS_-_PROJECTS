//    __author__ = "M.Satya Amarkant"
//    __version__ = "1.0"
//    __credits__ = ["M.SATYA AMARKANT"]
//    __license__ = "Private Domain :  Requires Approval for the use from M.SATYA AMARKANT"
//    __maintainer__ = "M.Satya Amarkant"
//    __contact__ = "amarkant23394@gmail.com"
//    __status__ = "Development"
// ConsoleApplication2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <time.h>
#include <sys/timeb.h>  // for timeb struct
#include <random>
#include <map>
#include <unordered_map>

using namespace std;

#define _CRT_SECURE_NO_WARNINGS 1

const int LOCKED = 2;
const int OCCUPIED = 1;
const int FREE = 0;

/********************** GLOBAL and STATIC VARIABLES ********************************************/
int cells_num = 0;		//number of cells
int nets_num = 0;		//numbers of edges
int cell_len = 6;		//length of the cell
int itr_limit = 10;		//iteration limit
int term_height = 9;	//Blocking the terminal height
int term_width = 11;	//Blocking the terminal width
int total_route_length = 0;	//Total route length
int sp_grid_col = 0;	//No. of grid coloumns
int sp_grid_row = 0;	//No. of grid rows
int total_vias = 0;		//No. of VIAS


/* NET AND TERMINAL CONNECTION REPRESENTATION */
typedef pair<unsigned int, unsigned int> term_conn;
typedef pair<int, int> x_y_cord;
map <unsigned int, term_conn> net_connection;

/* structure used for placement and cell information*/
struct cell_info
{
	x_y_cord left_top;
	x_y_cord right_top;
	x_y_cord left_bottom;
	x_y_cord right_bottom;
	int cell_num;
	int status = FREE;
};

/* Structure used for routing and number propagation*/
struct route_cell_info
{
	short int dist = 0;
	short int incr = 0;
	short int routed = 0;
};
/************************************************************************************/


/********************* FUNCTION DECLARATIONS ****************************************/
static vector<pair<pair<int, int>, int>> wave_prop(x_y_cord src, x_y_cord dest, vector<vector<struct route_cell_info>> p_grid_route1, vector<vector<struct route_cell_info>> p_grid_route2, int row, int col, int net_value, FILE* output_fd);
static void route_func(vector<vector<short int>>& p_spaced_grid_mat, vector<struct cell_info>& p_meta_data, FILE* output_fd, vector<vector<term_conn>>& p_conc_vec);
static void placement_func(FILE* input_fd, FILE* output_fd);
static void initial_grid_place_func(struct cell_info& p_meta_data, vector<vector<short int>>& p_grid_mat);
static bool cmp_pair(pair<int, int>& a, pair<int, int>& b);
static void cell_fill(struct cell_info p_meta_data, vector<vector<short int>>& p_grid_mat, int val);
static void free_all_cells(vector<struct cell_info>& p_meta_data);
static void update_meta_data(struct cell_info& p_meta_data, vector<vector<short int>>& p_grid_mat, int seed_cell, x_y_cord shifted_target_cord);
static x_y_cord search_for_vacant_cell(struct cell_info p_meta_data, vector<vector<short int>>& p_grid_mat);
static int calculating_wire_length(vector<struct cell_info>& p_meta_data, vector<vector<term_conn>>& p_conc_vec);
static void mag_write_func_place(FILE* output_fd, vector<struct cell_info>& p_meta_data);
static x_y_cord terminal_addr_cal(struct cell_info p_meta_data, int terminal_num);
static void spacing_adjustment(int& row_space, int& col_space);
/************************************************************************************/

/* ADJUSTING THE SPACE ACCORDING TO THE NETS NUM AFTER PLACEMENT */
static void spacing_adjustment(int& row_space, int& col_space)
{
	if (nets_num <= 50)
	{
		col_space = 20;
		row_space = 20;
		term_height = 5;
		term_width = 7;
	}
	else if (nets_num <= 100)
	{
		col_space = 30;
		row_space = 30;
		term_height = 5;
		term_width = 7;
	}
	else if (nets_num <= 800)
	{
		col_space = 50;
		row_space = 50;
		term_height = 5;
		term_width = 7;
	}
	else if (nets_num <= 1000)
	{
		col_space = 70;
		row_space = 70;
		term_height = 8;
		term_width = 10;
	}
	else if (nets_num <= 1200)
	{
		col_space = 80;
		row_space = 80;
		term_height = 9;
		term_width = 11;
	}
	else if (nets_num <= 1500)
	{
		col_space = 90;
		row_space = 90;
		term_height = 9;
		term_width = 11;
	}
	else if (nets_num <= 2000)
	{
		col_space = 110;
		row_space = 110;
		term_height = 9;
		term_width = 11;
	}
	else if (nets_num <= 5000)
	{
		col_space = 250;
		row_space = 250;
		term_height = 15;
		term_width = 17;
	}
	else
	{
		col_space = 100;
		row_space = 100;
		term_height = 9;
		term_width = 11;
	}
}

/* THIS FUNCTION IS USED FOR RETURN THE CO ORDINATES OF THE TERMINAL OF A PARTICULAR CELL */
static x_y_cord terminal_addr_cal(struct cell_info p_meta_data, int terminal_num)
{
	x_y_cord term_addr;

	if (terminal_num == 1)
	{
		term_addr.first = p_meta_data.left_top.first + 1;
		term_addr.second = p_meta_data.left_top.second + 1;
		return term_addr;
	}

	if (terminal_num == 2)
	{
		term_addr.first = p_meta_data.right_top.first - 1;
		term_addr.second = p_meta_data.right_top.second + 1;
		return term_addr;
	}

	if (terminal_num == 3)
	{
		term_addr.first = p_meta_data.left_bottom.first + 1;
		term_addr.second = p_meta_data.left_bottom.second - 1;
		return term_addr;
	}

	if (terminal_num == 4)
	{
		term_addr.first = p_meta_data.right_bottom.first - 1;
		term_addr.second = p_meta_data.right_bottom.second - 1;
		return term_addr;
	}
}

/* THIS FUNCTION IS USED TO WRITE PLACEMENT CELL DATA TO THE OUTPUT MAG FILE */
static void mag_write_func_place(FILE* output_fd, vector<struct cell_info>& p_meta_data)
{
	fprintf(output_fd, "magic\ntech scmos\ntimestamp\n");

	for (int i = 0; i < p_meta_data.size(); i++)
	{
		fprintf(output_fd, "<< pdiffusion >>\n");
		fprintf(output_fd, "rect %d %d %d %d\n", p_meta_data[i].left_bottom.first, p_meta_data[i].left_bottom.second, p_meta_data[i].right_top.first+1, p_meta_data[i].right_top.second+1);
		fprintf(output_fd, "<< labels >>\n");
		fprintf(output_fd, "rlabel pdiffusion %d %d %d %d 0 cell #%d\n", p_meta_data[i].left_bottom.first, p_meta_data[i].left_bottom.second, p_meta_data[i].right_top.first+1, p_meta_data[i].right_top.second+1, i+1);
		fprintf(output_fd, "<< polysilicon >>\n");
		fprintf(output_fd, "rect %d %d %d %d\n", p_meta_data[i].left_top.first+1, p_meta_data[i].left_top.second+1, p_meta_data[i].left_top.first+2, p_meta_data[i].left_top.second + 2); //T1
		fprintf(output_fd, "rect %d %d %d %d\n", p_meta_data[i].right_top.first-1, p_meta_data[i].right_top.second+1, p_meta_data[i].right_top.first, p_meta_data[i].right_top.second+2); //T2
		fprintf(output_fd, "rect %d %d %d %d\n", p_meta_data[i].left_bottom.first +1 , p_meta_data[i].left_bottom.second - 1, p_meta_data[i].left_bottom.first+2, p_meta_data[i].left_bottom.second); //T3
		fprintf(output_fd, "rect %d %d %d %d\n", p_meta_data[i].right_bottom.first-1, p_meta_data[i].right_bottom.second-1, p_meta_data[i].right_bottom.first, p_meta_data[i].right_bottom.second); //T4
		fprintf(output_fd, "<< labels >>\n");
		fprintf(output_fd, "rlabel polysilicon %d %d %d %d 0 T1\n", p_meta_data[i].left_top.first + 1, p_meta_data[i].left_top.second + 1, p_meta_data[i].left_top.first + 2, p_meta_data[i].left_top.second + 2); //T1
		fprintf(output_fd, "rlabel polysilicon %d %d %d %d 0 T2\n", p_meta_data[i].right_top.first - 1, p_meta_data[i].right_top.second + 1, p_meta_data[i].right_top.first, p_meta_data[i].right_top.second + 2); //T2
		fprintf(output_fd, "rlabel polysilicon %d %d %d %d 0 T3\n", p_meta_data[i].left_bottom.first + 1, p_meta_data[i].left_bottom.second - 1, p_meta_data[i].left_bottom.first + 2, p_meta_data[i].left_bottom.second); //T3
		fprintf(output_fd, "rlabel polysilicon %d %d %d %d 0 T4\n", p_meta_data[i].right_bottom.first - 1, p_meta_data[i].right_bottom.second - 1, p_meta_data[i].right_bottom.first, p_meta_data[i].right_bottom.second); //T4
	}
	//fprintf(output_fd, "<< end >>\n");
}

/* Calculating approximate wire length of the placement of the connected vectors using semi perimeter function */
static int calculating_wire_length(vector<struct cell_info>& p_meta_data, vector<vector<term_conn>>& p_conc_vec)
{
	int wire_length = 0;
	int min_x;
	int min_y;
	int max_x;

	int max_y;

	for (int i = 0; i < p_conc_vec.size(); i++)
	{
		min_x = p_meta_data[i].left_top.first;
		min_y = p_meta_data[i].left_top.second;
		max_x = p_meta_data[i].left_top.first;
		max_y = p_meta_data[i].left_top.second;

		for (int j = 0; j < p_conc_vec[i].size(); j++)
		{
			int temp = p_conc_vec[i][j].first;
			if (p_meta_data[temp].left_top.first > max_x)
				max_x = p_meta_data[temp].left_top.first;
			if (p_meta_data[temp].left_top.first < min_x)
				min_x = p_meta_data[temp].left_top.first;
			if (p_meta_data[temp].left_top.second > max_y)
				max_y = p_meta_data[temp].left_top.second;
			if (p_meta_data[temp].left_top.second < min_y)
				min_y = p_meta_data[temp].left_top.second;
		}
		wire_length += max_y - min_y + max_x - min_x;
	}

	return wire_length;
}

/* Checks for the nearest vacant cell and provides the */
static x_y_cord search_for_vacant_cell(struct cell_info p_meta_data, vector<vector<short int>>& p_grid_mat)
{
	int free_place_check = 0;
	int counter = 1;
	int temp;
	x_y_cord new_cord;
	int temp_addr_cal;

	x_y_cord left_cell;
	x_y_cord right_cell;
	x_y_cord top_cell;
	x_y_cord down_cell;

	x_y_cord left_top_diagonal_cal;

	x_y_cord side_cell_cal;

	while (true)
	{
		//calculating left cell
		temp_addr_cal = cell_len * counter;
		temp_addr_cal = p_meta_data.left_top.second - temp_addr_cal;
		left_cell = make_pair(p_meta_data.left_top.first, temp_addr_cal);

		//calculating top cell
		temp_addr_cal = cell_len * counter;
		temp_addr_cal = p_meta_data.left_top.first - temp_addr_cal;
		top_cell = make_pair(temp_addr_cal, p_meta_data.left_top.second);

		//calculating down cell
		temp_addr_cal = cell_len * counter;
		temp_addr_cal = p_meta_data.left_top.first + temp_addr_cal;
		down_cell = make_pair(temp_addr_cal, p_meta_data.left_top.second);

		//calculating right cell
		temp_addr_cal = cell_len * counter;
		temp_addr_cal = p_meta_data.left_top.second + temp_addr_cal;
		right_cell = make_pair(p_meta_data.left_top.first, temp_addr_cal);

		left_top_diagonal_cal = make_pair(top_cell.first, left_cell.second);

		int check = 1;
		int square_counter = 0;
		while (check == 1)
		{
			int tmp = cell_len * square_counter;

			//left square up side
			temp_addr_cal = left_cell.first - tmp;
			side_cell_cal = make_pair(temp_addr_cal, left_cell.second);
			
			//loop condition
			if (side_cell_cal == left_top_diagonal_cal)
				check = 0;

			if(side_cell_cal.first >= 0 && side_cell_cal.second >= 0)
				if (p_grid_mat[side_cell_cal.first][side_cell_cal.second] == -1)
					return side_cell_cal;


			//left square down side
			temp_addr_cal = left_cell.first + tmp;
			side_cell_cal = make_pair(temp_addr_cal, left_cell.second);

			if ((side_cell_cal.first <= (p_grid_mat.size() - cell_len)) && side_cell_cal.second >= 0)
				if (p_grid_mat[side_cell_cal.first][side_cell_cal.second] == -1)
					return side_cell_cal;

			//right square up side
			temp_addr_cal = right_cell.first - tmp;
			side_cell_cal = make_pair(temp_addr_cal, right_cell.second);

			if (side_cell_cal.first >= 0 && side_cell_cal.second <= (p_grid_mat[0].size()-cell_len))
				if (p_grid_mat[side_cell_cal.first][side_cell_cal.second] == -1)
					return side_cell_cal;

			//right square down side
			temp_addr_cal = right_cell.first + tmp;
			side_cell_cal = make_pair(temp_addr_cal, right_cell.second);
			
			if ((side_cell_cal.first <= (p_grid_mat.size() - cell_len)) && side_cell_cal.second <= (p_grid_mat[0].size() - cell_len))
				if (p_grid_mat[side_cell_cal.first][side_cell_cal.second] == -1)
					return side_cell_cal;

			//down square left side
			temp_addr_cal = down_cell.second - tmp;
			side_cell_cal = make_pair(down_cell.first,temp_addr_cal);

			if ((side_cell_cal.first <= (p_grid_mat.size() - cell_len)) && side_cell_cal.second >= 0)
				if (p_grid_mat[side_cell_cal.first][side_cell_cal.second] == -1)
					return side_cell_cal;

			//down square right side
			temp_addr_cal = down_cell.second + tmp;
			side_cell_cal = make_pair(down_cell.first, temp_addr_cal);

			if ((side_cell_cal.first <= (p_grid_mat.size() - cell_len)) && side_cell_cal.second  <= (p_grid_mat[0].size() - cell_len))
				if (p_grid_mat[side_cell_cal.first][side_cell_cal.second] == -1)
					return side_cell_cal;

			//top square left side
			temp_addr_cal = top_cell.second - tmp;
			side_cell_cal = make_pair(top_cell.first, temp_addr_cal);

			if ((side_cell_cal.first >= 0) && side_cell_cal.second >= 0)
				if (p_grid_mat[side_cell_cal.first][side_cell_cal.second] == -1)
					return side_cell_cal;

			//top square right side
			temp_addr_cal = top_cell.second + tmp;
			side_cell_cal = make_pair(top_cell.first, temp_addr_cal);

			if ((side_cell_cal.first >= 0) && side_cell_cal.second <= (p_grid_mat[0].size() - cell_len))
				if (p_grid_mat[side_cell_cal.first][side_cell_cal.second] == -1)
					return side_cell_cal;

			square_counter++;
		}
		counter++;
	} 
}

/* Updating the meta data and then updating the same in grid */
static void update_meta_data(struct cell_info& p_meta_data, vector<vector<short int>>& p_grid_mat, int seed_cell, x_y_cord shifted_target_cord)
{
	//updating remaining data on reference to left top
	p_meta_data.left_top = shifted_target_cord;
	p_meta_data.right_top = make_pair(p_meta_data.left_top.first, p_meta_data.left_top.second + cell_len-1);
	p_meta_data.left_bottom = make_pair(p_meta_data.left_top.first + cell_len-1, p_meta_data.left_top.second);
	p_meta_data.right_bottom = make_pair(p_meta_data.left_top.first + cell_len-1, p_meta_data.left_top.second + cell_len-1);

	//updating the grid accordingly
	for (int i = 0; i < cell_len; i++)
	{
		for (int j = 0; j < cell_len; j++)
		{
			p_grid_mat[p_meta_data.left_top.first + i][p_meta_data.left_top.second + j] = seed_cell;
		}
	}
}

/* Unlocking all cell locations */
static void free_all_cells(vector<struct cell_info>& p_meta_data)
{
	for (int i = 0; i < cells_num; i++)
	{
		p_meta_data[i].status = FREE;
	}
}

 /* To fill the cell in grid with the parameter value */
static void cell_fill(struct cell_info p_meta_data, vector<vector<short int>>& p_grid_mat, int val)
{
	for (int i = 0; i < cell_len; i++)
	{
		for (int j = 0; j < cell_len; j++)
		{
			p_grid_mat[p_meta_data.left_top.first + i][p_meta_data.left_top.second + j] = val;
		}
	}
}

/* To compair two pairs on it's second value */
static bool cmp_pair(pair<int, int>& a, pair<int, int>& b)
{
	return a.second > b.second;
}

/* To Intially place all cells in serial order */
static void initial_grid_place_func(struct cell_info& p_meta_data, vector<vector<short int>> &p_grid_mat)
{
	p_meta_data.right_top = make_pair(p_meta_data.left_top.first, p_meta_data.left_top.second + cell_len);
	p_meta_data.left_bottom = make_pair(p_meta_data.left_top.first + cell_len, p_meta_data.left_top.second);
	p_meta_data.right_bottom = make_pair(p_meta_data.left_top.first + cell_len, p_meta_data.left_top.second + cell_len);

	for (int i = 0; i < cell_len; i++)
	{
		for (int j = 0; j < cell_len; j++)
		{
			p_grid_mat[p_meta_data.left_top.first + i][p_meta_data.left_top.second + j] = p_meta_data.cell_num;
		}
	}
}


static void placement_func(FILE* input_fd, FILE* output_fd)
{

#if 1
	//Netlist is read from file and vector is filled accordingly
	//file = input_fd = file buffer
	//vector = the connection between nodes = diagonnaly symmetric matrix

	char file_buffer[256];	//a buffer for reading input
	char* trunc_str;		//token for input parsing
	int f_net;				//for parsing net number
	int f_tok;				//for parsing first read node
	int src_terminal;			//for parssing first terminal
	int s_tok;				//for parsing second read node
	int des_terminal;			//for parssing second terminal

	int temp_nets = 0;
	vector<vector<term_conn>> conc_vec;
	conc_vec.resize(cells_num);			//can't declare with variable so resizing it //reference stack overflow

	/******************* FILE READING AND ARRANGING IN RESPECTIVE DATA STRUCTURE *********************************************/
	while (fgets(file_buffer, 256, input_fd)) {
		if (temp_nets >= nets_num)
			break;

		trunc_str = strtok(file_buffer, " \t\n");	//reading first net num
		if (trunc_str != NULL) f_net = atoi(trunc_str);
		else break;									//on error break out

		trunc_str = strtok(NULL, " \t\n");	//reading first token
		if (trunc_str != NULL) f_tok = atoi(trunc_str);
		else break;									//on error break out

		trunc_str = strtok(NULL, " \t\n");	//reading first terminal
		if (trunc_str != NULL) src_terminal = atoi(trunc_str);
		else break;									//on error break out

		trunc_str = strtok(NULL, " \t\n");	//reading second token
		if (trunc_str != NULL) s_tok = atoi(trunc_str);
		else break;

		trunc_str = strtok(NULL, " \t\n");	//reading second terminal
		if (trunc_str != NULL) des_terminal = atoi(trunc_str);
		else break;									//on error break out

		conc_vec[f_tok - 1].push_back(make_pair((s_tok - 1), f_net - 1));			//first node connection
		net_connection[f_net - 1] = make_pair(src_terminal, des_terminal);
		if (f_tok != s_tok) {
			conc_vec[s_tok - 1].push_back(make_pair((f_tok - 1), f_net - 1 + nets_num));			//second node connection
			net_connection[f_net - 1 + nets_num] = make_pair(des_terminal, src_terminal);
		}
		temp_nets++;
	}
#endif
	/**********************************************************************************************************/

	/****************************DECLARATION OF GRID AND INITIAL VALUES SUCH SPACING **************************/
	int col_count = (int)(ceil(sqrt(cells_num)));
	int row_count = (int)(ceil(sqrt(cells_num)));

	int col_space = 0;
	int row_space = 0;

	int grid_col = (col_count * cell_len) + ((col_count - 1) * col_space);
	int grid_row = (row_count * cell_len) + ((row_count - 1) * row_space);

	vector<vector<short int>> grid_mat(grid_row, vector<short int>(grid_col, -1));
	vector<struct cell_info> meta_data;
	meta_data.resize(cells_num);
	/**********************************************************************************************************/

	/****************************INITIAL PLACEMENT**********************************************/
	{
		x_y_cord grid_counter = make_pair(0, 0);

		for (int i = 0; i < cells_num; i++)
		{
			if ((grid_counter.second + cell_len) <= grid_col)
			{
				meta_data[i].left_top = grid_counter;
			}
			else
			{
				grid_counter.second = 0;
				grid_counter.first = grid_counter.first + cell_len + row_space;
				meta_data[i].left_top = grid_counter;
			}

			meta_data[i].cell_num = i;
			initial_grid_place_func(meta_data[i], grid_mat);

			grid_counter.second = grid_counter.second + cell_len + col_space;
		}
	}

	cout << endl << "THe intial placement wirelength (without spacing with cell expanded to given cell length) =  " << calculating_wire_length(meta_data, conc_vec);

	/******** computing the connectivity of each cell ********************/
#if 1
	// Declare vector of pairs
	vector<pair<int, int> > connection_map;

	//Forimg the vector of pairs of index and connectivity size
	for (int i = 0; i < cells_num; i++)
	{
		int temp_size = conc_vec[i].size();
		connection_map.push_back(make_pair(i, temp_size));
	}

	// Sort using comparator function
	sort(connection_map.begin(), connection_map.end(), cmp_pair);

#endif
	/**********************************************************************/

	/*************IMPLEMENTATION OF THE ALGORITHM**************************/
	int itr_count = 0;				//iteration count
	int seed_cell_counter = 0;		//seed cell
	int seed_cell_list_rec;			//recovering the seed cell for next iteration
	bool end_ripple;				//ripple value
	int target_pointed_cell;		//grid location cell referred by the target point
	x_y_cord shifted_target_cord;	//target pointed shifted to the uniform place
	int abort_limit = (int)(0.2*cells_num);			//abort limit
	int abort_count = 0;			//abort count

#if 1
	while (itr_count < itr_limit)
	{
		/* If there is no cell in the list is free */
		if (seed_cell_counter >= cells_num)
		{
			seed_cell_counter = 0;
			free_all_cells(meta_data);
			itr_count++;
			continue;
		}

		//searching for the next free cell
		while (seed_cell_counter < cells_num && (meta_data[connection_map[seed_cell_counter].first].status == LOCKED || (conc_vec[connection_map[seed_cell_counter].first].size() == 0)))
		{
			seed_cell_counter++;
		}

		if (seed_cell_counter >= cells_num)
			continue;

		//vacanting the seed cell position in grid
		meta_data[connection_map[seed_cell_counter].first].status = FREE;
		cell_fill(meta_data[connection_map[seed_cell_counter].first], grid_mat, -1);

		seed_cell_list_rec = seed_cell_counter;

		end_ripple = false;

		while (end_ripple == false)
		{
			/*Computing target point for the seed cell*/
			x_y_cord target_point = make_pair(0, 0);

			for (int i = 0; i < conc_vec[connection_map[seed_cell_counter].first].size(); i++)
			{
				int temp_conn_cell = conc_vec[connection_map[seed_cell_counter].first][i].first;					//particular connected destination to seed cell
				target_point.first += meta_data[temp_conn_cell].left_top.first;		//calculating summation of xiwi
				target_point.second += meta_data[temp_conn_cell].left_top.second;	//calculating summation of yiwi
			}

			if (conc_vec[connection_map[seed_cell_counter].first].size() != 0)
			{
				target_point.first = target_point.first / (conc_vec[connection_map[seed_cell_counter].first].size());		//calculating x0
				target_point.second = target_point.second / (conc_vec[connection_map[seed_cell_counter].first].size());	//calculation y0
			}
			else
			{
				target_point = meta_data[connection_map[seed_cell_counter].first].left_top;
			}

			target_pointed_cell = grid_mat[target_point.first][target_point.second];


			/*********CASES********************/
			
			/* Target location is same as seed cell location */
			if (target_pointed_cell == connection_map[seed_cell_counter].first)
			{
				cell_fill(meta_data[connection_map[seed_cell_counter].first], grid_mat, target_pointed_cell);
				end_ripple = true;
				abort_count = 0;
				if (conc_vec[connection_map[seed_cell_counter].first].size() != 0)
					meta_data[connection_map[seed_cell_counter].first].status = LOCKED;
			}

			/* Target location is already vacant */
			else if (target_pointed_cell == -1)
			{
				//search location and fill it up with seed cell data
				int x_cord = (target_point.first) / cell_len;
				x_cord = x_cord * cell_len;
				int y_cord = (target_point.second) / cell_len;
				y_cord = y_cord * cell_len;
				shifted_target_cord = make_pair(x_cord, y_cord);
				update_meta_data(meta_data[connection_map[seed_cell_counter].first], grid_mat, connection_map[seed_cell_counter].first, shifted_target_cord);
				end_ripple = true;
				abort_count = 0;
				meta_data[connection_map[seed_cell_counter].first].status = LOCKED;
			}

			/* Target location is already occupied and lcoked */
			else if (meta_data[target_pointed_cell].status == LOCKED)
			{
#if 1
				shifted_target_cord = search_for_vacant_cell(meta_data[target_pointed_cell], grid_mat);
				update_meta_data(meta_data[connection_map[seed_cell_counter].first], grid_mat, connection_map[seed_cell_counter].first, shifted_target_cord);
				meta_data[connection_map[seed_cell_counter].first].status = LOCKED;
				end_ripple = true;
				abort_count += 1;
				if (abort_count > abort_limit)
				{
					free_all_cells(meta_data);
					itr_count += 1;
					seed_cell_list_rec = 0;
					seed_cell_counter = 0;
				}
#endif
			}

			/* Target location is already occupied */
			else if (target_pointed_cell != -1)
			{
#if 1
				meta_data[connection_map[seed_cell_counter].first] = meta_data[target_pointed_cell];
				cell_fill(meta_data[connection_map[seed_cell_counter].first], grid_mat, connection_map[seed_cell_counter].first);
				meta_data[connection_map[seed_cell_counter].first].status = LOCKED;
				end_ripple = false;
				abort_count = 0;

				seed_cell_counter = 0;

				for (auto& it : connection_map)
				{
					if (it.first == target_pointed_cell)
						break;

					seed_cell_counter++;
				}
#endif
			}
		}
		seed_cell_counter = seed_cell_list_rec;
	}
#endif
	/*********************************************************************/
	cout << endl << "The Final placement wirelength (without spacing and considered only required cell*cell limitations)  =  " << calculating_wire_length(meta_data, conc_vec);

	spacing_adjustment(col_space,row_space);

	sp_grid_col = (col_count * cell_len) + ((col_count + 1) * col_space);
	sp_grid_row = (row_count * cell_len) + ((row_count + 1) * row_space);

	int non_sp_grid_col = 0;
	int non_sp_grid_row = 0;
	int non_sp_cell_point;
	
	/* CHANGING THE GRID INTO SPACED GRID ACCORDING TO NETS NUM */
	vector<vector<short int>> spaced_grid_mat(sp_grid_row, vector<short int>(sp_grid_col, -1));
#if 1
	for (int i = row_space; i < sp_grid_row; i = i + row_space + cell_len)
	{
		non_sp_grid_col = 0;
		for (int j = col_space; j < sp_grid_col; j = j + col_space + cell_len)
		{
			non_sp_cell_point = grid_mat[non_sp_grid_row][non_sp_grid_col];
#if 1
			if (non_sp_cell_point >= 0)
			{
				meta_data[non_sp_cell_point].left_top = make_pair(i, j);
				meta_data[non_sp_cell_point].right_top = make_pair(meta_data[non_sp_cell_point].left_top.first, meta_data[non_sp_cell_point].left_top.second + cell_len-1);
				meta_data[non_sp_cell_point].left_bottom = make_pair(meta_data[non_sp_cell_point].left_top.first + cell_len-1, meta_data[non_sp_cell_point].left_top.second);
				meta_data[non_sp_cell_point].right_bottom = make_pair(meta_data[non_sp_cell_point].left_top.first + cell_len-1, meta_data[non_sp_cell_point].left_top.second + cell_len-1);

				//updating the grid accordingly
				for (int i = 0; i < cell_len; i++)
				{
					for (int j = 0; j < cell_len; j++)
					{
						spaced_grid_mat[meta_data[non_sp_cell_point].left_top.first + i][meta_data[non_sp_cell_point].left_top.second + j] = non_sp_cell_point;
					}
				}
			}
#endif
			non_sp_grid_col = non_sp_grid_col + cell_len;
		}
		non_sp_grid_row = non_sp_grid_row + cell_len;
	}
#endif

	/* CLEARING THE PREVIOUS UNSPACED GRID */
	grid_mat.clear();

#if 1
	/* NUMBERING THE TERMINALS AND CELLS BOUNDARIES */
	for (int i = 0; i < meta_data.size(); i++)
	{
		for (int j = 0; j < cell_len + 2; j++)
		{
			//numbering top boundary
			spaced_grid_mat[meta_data[i].left_top.first - 1][meta_data[i].left_top.second - 1 + j] = i;

			//numbering left boundary
			spaced_grid_mat[meta_data[i].left_top.first - 1 + j][meta_data[i].left_top.second - 1] = i;

			//numbering right boundary
			spaced_grid_mat[meta_data[i].right_top.first - 1 + j][meta_data[i].right_top.second + 1] = i;

			//numbering bottom boundary
			spaced_grid_mat[meta_data[i].left_top.first + 1][meta_data[i].left_top.second - 1 + j] = i;

		}
#if 1
		//terminal numbering
		for (int k = 0; k < term_height; k++)
		{
			for (int j = 0; j < term_width; j++)
			{
				//terminal 1
				spaced_grid_mat[meta_data[i].left_top.first - 1 - k][meta_data[i].left_top.second + 2 - j] = i;

				//terminal 2
				spaced_grid_mat[meta_data[i].right_top.first - 1 - k][meta_data[i].right_top.second - 2 + j] = i;

				//terminal 3
				spaced_grid_mat[meta_data[i].left_bottom.first + 1 + k ][meta_data[i].left_bottom.second + 2 - j] = i;

				//terminal 3
				spaced_grid_mat[meta_data[i].right_bottom.first + 1 + k][meta_data[i].right_bottom.second - 2 + j] = i;
			}
		}
#endif
	}
#endif

	vector<vector<short int>> outtrans(spaced_grid_mat[0].size(), vector<short int>(spaced_grid_mat.size()));

	for (int i = 0; i < spaced_grid_mat.size(); ++i)
		for (int j = 0; j < spaced_grid_mat[0].size(); ++j)
			outtrans[j][i] = spaced_grid_mat[i][j];

	//clearing the un-transpose spaced grid
	spaced_grid_mat.clear();

	x_y_cord temp_cord_left_top;
	x_y_cord temp_cord_right_bottom;

	/* UPDATE META DATA */
	for (int i = 0; i < meta_data.size(); i++)
	{
		temp_cord_left_top = meta_data[i].left_top;
		temp_cord_right_bottom = meta_data[i].right_bottom;
		meta_data[i].right_bottom = meta_data[i].left_bottom;
		meta_data[i].left_top = meta_data[i].right_top;
		meta_data[i].right_top = temp_cord_right_bottom;
		meta_data[i].left_bottom = temp_cord_left_top;
	}

	/* WRITING THE SPACED PLACEMENT INTO MAG FILE */
	mag_write_func_place(output_fd, meta_data);

	/* CALLING FOR ROUTING*/
	route_func(outtrans, meta_data, output_fd, conc_vec);
}

/* This function is used for routing in the given grid (in 2 metal space) */
static void route_func(vector<vector<short int>>& p_spaced_grid_mat, vector<struct cell_info>& p_meta_data, FILE* output_fd, vector<vector<term_conn>>& p_conc_vec)
{
	x_y_cord src, dest;
	int grid_col = p_spaced_grid_mat[0].size();
	int grid_row = p_spaced_grid_mat.size();

	vector<vector<struct route_cell_info>> grid_route1(grid_row, vector<struct route_cell_info>(grid_col));
	vector<vector<struct route_cell_info>> grid_route2(grid_row, vector<struct route_cell_info>(grid_col));

	/* BLOCKING THE CELLS AND TERMINALS */
	for (int i = 0; i < grid_row; i++)
	{
		for (int j = 0; j < grid_col; j++)
		{
			if (p_spaced_grid_mat[i][j] != -1)
			{
				grid_route1[i][j].routed = 1;
				grid_route2[i][j].routed = 1;
			}
		}
	}

	int src_cell;
	int des_cell;
	int net_num;
	vector<pair<pair<int, int>, int>> path_coord;

	for (int i = 0; i < p_conc_vec.size(); i++)
	{
		src_cell = i;

		for (int j = 0; j < p_conc_vec[i].size(); j++)
		{
			des_cell = p_conc_vec[i][j].first;
			net_num = p_conc_vec[i][j].second;
			if (net_num < nets_num)
			{
				src = terminal_addr_cal(p_meta_data[src_cell], net_connection[net_num].first);
				dest = terminal_addr_cal(p_meta_data[des_cell], net_connection[net_num].second);
#if 1
				//terminal free
				for (int k = 0; k < term_height; k++)
				{
					for (int j = 0; j < term_width; j++)
					{
						//source terminal
						if (net_connection[net_num].first == 1)
						{
							grid_route1[src.first - (term_width - 2) + j][src.second + k].routed = (grid_route1[src.first - (term_width - 2) + j][src.second + k].routed == 1) ? 0 : grid_route1[src.first - (term_width - 2) + j][src.second + k].routed;
							grid_route2[src.first - (term_width - 2) + j][src.second + k].routed = (grid_route2[src.first - (term_width - 2) + j][src.second + k].routed == 1) ? 0 : grid_route2[src.first - (term_width - 2) + j][src.second + k].routed;
						}

						if (net_connection[net_num].first == 2)
						{
							grid_route1[src.first - 1 + j][src.second + k].routed = (grid_route1[src.first - 1 + j][src.second + k].routed == 1) ? 0 : grid_route1[src.first - 1 + j][src.second + k].routed;
							grid_route2[src.first - 1 + j][src.second + k].routed = (grid_route2[src.first - 1 + j][src.second + k].routed == 1) ? 0 : grid_route2[src.first - 1 + j][src.second + k].routed;
						}

						if (net_connection[net_num].first == 3)
						{
							grid_route1[src.first - (term_width - 2) + j][src.second - k].routed = (grid_route1[src.first - (term_width - 2) + j][src.second - k].routed == 1) ? 0 : grid_route1[src.first - (term_width - 2) + j][src.second - k].routed;
							grid_route2[src.first - (term_width - 2) + j][src.second - k].routed = (grid_route2[src.first - (term_width - 2) + j][src.second - k].routed == 1) ? 0 : grid_route2[src.first - (term_width - 2) + j][src.second - k].routed;
						}

						if (net_connection[net_num].first == 4)
						{
							grid_route1[src.first - 1 + j][src.second - k].routed = (grid_route1[src.first - 1 + j][src.second - k].routed == 1) ? 0 : grid_route1[src.first - 1 + j][src.second - k].routed;
							grid_route2[src.first - 1 + j][src.second - k].routed = (grid_route2[src.first - 1 + j][src.second - k].routed == 1) ? 0 : grid_route2[src.first - 1 + j][src.second - k].routed;
						}

						//destination terminal
						if (net_connection[net_num].second == 1)
						{
							grid_route1[dest.first - (term_width - 2) + j][dest.second + k].routed = (grid_route1[dest.first - (term_width - 2) + j][dest.second + k].routed == 1) ? 0 : grid_route1[dest.first - (term_width - 2) + j][dest.second + k].routed;
							grid_route2[dest.first - (term_width - 2) + j][dest.second + k].routed = (grid_route2[dest.first - (term_width - 2) + j][dest.second + k].routed == 1) ? 0 : grid_route2[dest.first - (term_width - 2) + j][dest.second + k].routed;
						}

						if (net_connection[net_num].second == 2)
						{
							grid_route1[dest.first - 1 + j][dest.second + k].routed = (grid_route1[dest.first - 1 + j][dest.second + k].routed == 1) ? 0 : grid_route1[dest.first - 1 + j][dest.second + k].routed;
							grid_route2[dest.first - 1 + j][dest.second + k].routed = (grid_route2[dest.first - 1 + j][dest.second + k].routed == 1) ? 0 : grid_route2[dest.first - 1 + j][dest.second + k].routed;
						}

						if (net_connection[net_num].second == 3)
						{
							grid_route1[dest.first - (term_width - 2) + j][dest.second - k].routed = (grid_route1[dest.first - (term_width - 2) + j][dest.second - k].routed == 1) ? 0 : grid_route1[dest.first - (term_width - 2) + j][dest.second - k].routed;
							grid_route2[dest.first - (term_width - 2) + j][dest.second - k].routed = (grid_route2[dest.first - (term_width - 2) + j][dest.second - k].routed == 1) ? 0 : grid_route2[dest.first - (term_width - 2) + j][dest.second - k].routed;
						}

						if (net_connection[net_num].second == 4)
						{
							grid_route1[dest.first - 1 + j][dest.second - k].routed = (grid_route1[dest.first - 1 + j][dest.second - k].routed == 1) ? 0 : grid_route1[dest.first - 1 + j][dest.second - k].routed;
							grid_route2[dest.first - 1 + j][dest.second - k].routed = (grid_route2[dest.first - 1 + j][dest.second - k].routed == 1) ? 0 : grid_route2[dest.first - 1 + j][dest.second - k].routed;
						}
					}
				}
#endif
				path_coord = wave_prop(src, dest, grid_route1, grid_route2, grid_row, grid_col, net_num, output_fd);

#if 1
				//terminal lock
				for (int k = 0; k < term_height; k++)
				{
					for (int j = 0; j < term_width; j++)
					{
						//source terminal
						if (net_connection[net_num].first == 1)
						{
							if (grid_route1[src.first - (term_width - 2) + j][src.second + k].routed == 0)
								grid_route1[src.first - (term_width - 2) + j][src.second + k].routed = 1;

							if (grid_route2[src.first - (term_width - 2) + j][src.second + k].routed == 0)
								grid_route2[src.first - (term_width - 2) + j][src.second + k].routed = 1;
						}

						if (net_connection[net_num].first == 2)
						{
							if (grid_route1[src.first - 1 + j][src.second + k].routed == 0)
								grid_route1[src.first - 1 + j][src.second + k].routed = 1;
							if (grid_route2[src.first - 1 + j][src.second + k].routed == 0)
								grid_route2[src.first - 1 + j][src.second + k].routed = 1;
						}

						if (net_connection[net_num].first == 3)
						{
							if (grid_route1[src.first - (term_width - 2) + j][src.second - k].routed == 0)
								grid_route1[src.first - (term_width - 2) + j][src.second - k].routed = 1;
							if (grid_route2[src.first - (term_width - 2) + j][src.second - k].routed == 0)
								grid_route2[src.first - (term_width - 2) + j][src.second - k].routed = 1;
						}

						if (net_connection[net_num].first == 4)
						{
							if (grid_route1[src.first - 1 + j][src.second - k].routed == 0)
								grid_route1[src.first - 1 + j][src.second - k].routed = 1;
							if (grid_route2[src.first - 1 + j][src.second - k].routed == 0)
								grid_route2[src.first - 1 + j][src.second - k].routed = 1;
						}

						//destination terminal
						if (net_connection[net_num].second == 1)
						{
							if (grid_route1[dest.first - (term_width - 2) + j][dest.second + k].routed == 0)
								grid_route1[dest.first - (term_width - 2) + j][dest.second + k].routed = 1;
							if (grid_route2[dest.first - (term_width - 2) + j][dest.second + k].routed == 0)
								grid_route2[dest.first - (term_width - 2) + j][dest.second + k].routed = 1;
						}

						if (net_connection[net_num].second == 2)
						{
							if (grid_route1[dest.first - 1 + j][dest.second + k].routed == 0)
								grid_route1[dest.first - 1 + j][dest.second + k].routed = 1;
							if (grid_route2[dest.first - 1 + j][dest.second + k].routed == 0)
								grid_route2[dest.first - 1 + j][dest.second + k].routed = 1;
						}

						if (net_connection[net_num].second == 3)
						{
							if (grid_route1[dest.first - (term_width - 2) + j][dest.second - k].routed == 0)
								grid_route1[dest.first - (term_width - 2) + j][dest.second - k].routed = 1;
							if (grid_route2[dest.first - (term_width - 2) + j][dest.second - k].routed == 0)
								grid_route2[dest.first - (term_width - 2) + j][dest.second - k].routed = 1;
						}

						if (net_connection[net_num].second == 4)
						{
							if (grid_route1[dest.first - 1 + j][dest.second - k].routed == 0)
								grid_route1[dest.first - 1 + j][dest.second - k].routed = 1;
							if (grid_route2[dest.first - 1 + j][dest.second - k].routed == 0)
								grid_route2[dest.first - 1 + j][dest.second - k].routed = 1;
						}
					}
				}
#endif

#if 1
				int layer;
				int prev_layer;

				for (int m = 0; m < path_coord.size(); m++)
				{
					if (m == 1)
						prev_layer = layer;

					if (path_coord[m].second == 1)
					{
						layer = 1;
						grid_route1[path_coord[m].first.first - 1][path_coord[m].first.second].routed = 2;
						grid_route1[path_coord[m].first.first][path_coord[m].first.second].routed = 2;
						grid_route1[path_coord[m].first.first + 1][path_coord[m].first.second].routed = 2;
					}
					else
					{
						layer = 2;
						grid_route2[path_coord[m].first.first][path_coord[m].first.second - 1].routed = 2;
						grid_route2[path_coord[m].first.first][path_coord[m].first.second].routed = 2;
						grid_route2[path_coord[m].first.first][path_coord[m].first.second + 1].routed = 2;
					}

					if (m != 0 && layer != prev_layer)
					{
						grid_route1[path_coord[m - 1].first.first][path_coord[m - 1].first.second].routed = 3;
						grid_route1[path_coord[m - 1].first.first - 1][path_coord[m - 1].first.second - 1].routed = 3;
						grid_route1[path_coord[m - 1].first.first - 1][path_coord[m - 1].first.second].routed = 3;
						grid_route1[path_coord[m - 1].first.first - 1][path_coord[m - 1].first.second + 1].routed = 3;
						grid_route1[path_coord[m - 1].first.first][path_coord[m - 1].first.second + 1].routed = 3;
						grid_route1[path_coord[m - 1].first.first + 1][path_coord[m - 1].first.second + 1].routed = 3;
						grid_route1[path_coord[m - 1].first.first + 1][path_coord[m - 1].first.second].routed = 3;
						grid_route1[path_coord[m - 1].first.first + 1][path_coord[m - 1].first.second - 1].routed = 3;
						grid_route1[path_coord[m - 1].first.first][path_coord[m - 1].first.second - 1].routed = 3;


						grid_route2[path_coord[m - 1].first.first][path_coord[m - 1].first.second].routed = 3;
						grid_route2[path_coord[m - 1].first.first - 1][path_coord[m - 1].first.second - 1].routed = 3;
						grid_route2[path_coord[m - 1].first.first - 1][path_coord[m - 1].first.second].routed = 3;
						grid_route2[path_coord[m - 1].first.first - 1][path_coord[m - 1].first.second + 1].routed = 3;
						grid_route2[path_coord[m - 1].first.first][path_coord[m - 1].first.second + 1].routed = 3;
						grid_route2[path_coord[m - 1].first.first + 1][path_coord[m - 1].first.second + 1].routed = 3;
						grid_route2[path_coord[m - 1].first.first + 1][path_coord[m - 1].first.second].routed = 3;
						grid_route2[path_coord[m - 1].first.first + 1][path_coord[m - 1].first.second - 1].routed = 3;
						grid_route2[path_coord[m - 1].first.first - 1][path_coord[m - 1].first.second].routed = 3;

						total_vias++;

						prev_layer = layer;
					}

				}
#endif
			}
		}
	}
	fprintf(output_fd, "<< end >>\n");
}

/* THIS FUNCTION PROPAGATE NUMBERS AND SEARCHES THE BEST ROUTE FOR SOURCE AND TERMINAL */
static vector<pair<pair<int, int>, int>> wave_prop(x_y_cord src, x_y_cord dest, vector<vector<struct route_cell_info>> p_grid_route1, vector<vector<struct route_cell_info>> p_grid_route2, int row, int col,int net_value, FILE* output_fd)
{
	vector<x_y_cord> present, next;
	int x = src.first;
	int y = src.second;
	int x_f = dest.first;
	int y_f = dest.second;
	int row_limit = 0, col_limit = 0;
	x_y_cord temp;
	vector<pair<pair<int, int>, int>> path_coord;

	int loop = 1, len = 1, route_len = 0;

	present.push_back(src); //To start propagation from src
	p_grid_route1[x][y].incr++; //To avoid the initial overwriting of the updated cells
	int loop_close = 0;
	while (loop)
	{
		next.clear();
		if (present.size() == 0)
		{
			route_len = len;
			break;
		}

		for (int i = 0; i < present.size(); i++)
		{
			x = present[i].first;
			y = present[i].second;
			{
				//All possible Propagating cases
				if (((x + 1) < row) && (p_grid_route1[x + 1][y].incr == 0))
				{
					if (p_grid_route1[x + 1][y].routed == 0) //To skip overwriting of the routed cells in grid1
						if (p_grid_route1[x + 1][y].dist == 0)
							p_grid_route1[x + 1][y].dist = len;
					if (p_grid_route2[x + 1][y].routed == 0) //To skip overwriting of the routed cells in grid2
						if (p_grid_route2[x + 1][y].dist == 0)
							p_grid_route2[x + 1][y].dist = len;
					if (p_grid_route1[x + 1][y].routed == 0 || p_grid_route2[x + 1][y].routed == 0)
					{
						p_grid_route1[x + 1][y].incr++;

					}
					if (p_grid_route2[x + 1][y].routed == 0)
						next.push_back(make_pair(x + 1, y));
				}

				if ((x - 1 >= 0) && (p_grid_route1[x - 1][y].incr == 0))
				{
					if (p_grid_route1[x - 1][y].routed == 0)
						if (p_grid_route1[x - 1][y].dist == 0)
							p_grid_route1[x - 1][y].dist = len;
					if (p_grid_route2[x - 1][y].routed == 0)
						if (p_grid_route2[x - 1][y].dist == 0)
							p_grid_route2[x - 1][y].dist = len;
					if (p_grid_route1[x - 1][y].routed == 0 || p_grid_route2[x - 1][y].routed == 0)
					{
						p_grid_route1[x - 1][y].incr++;
					}
					if (p_grid_route2[x - 1][y].routed == 0)
						next.push_back(make_pair(x - 1, y));
				}

				if ((y + 1 < col) && (p_grid_route1[x][y + 1].incr == 0))
				{
					if (p_grid_route1[x][y + 1].routed == 0)
						if (p_grid_route1[x][y + 1].dist == 0)
							p_grid_route1[x][y + 1].dist = len;
					if (p_grid_route2[x][y + 1].routed == 0)
						if (p_grid_route2[x][y + 1].dist == 0)
							p_grid_route2[x][y + 1].dist = len;
					if (p_grid_route1[x][y + 1].routed == 0 || p_grid_route2[x][y + 1].routed == 0)
					{
						p_grid_route1[x][y + 1].incr++;
					}
					if (p_grid_route1[x][y + 1].routed == 0)
						next.push_back(make_pair(x, y + 1));
				}

				if ((y - 1 >= 0) && (p_grid_route1[x][y - 1].incr == 0))
				{
					if (p_grid_route1[x][y - 1].routed == 0)
						if (p_grid_route1[x][y - 1].dist == 0)
							p_grid_route1[x][y - 1].dist = len;
					if (p_grid_route2[x][y - 1].routed == 0)
						if (p_grid_route2[x][y - 1].dist == 0)
							p_grid_route2[x][y - 1].dist = len;
					if (p_grid_route1[x][y - 1].routed == 0 || p_grid_route2[x][y - 1].routed == 0)
					{
						p_grid_route1[x][y - 1].incr++;
					}

					if (p_grid_route1[x][y - 1].routed == 0)
						next.push_back(make_pair(x, y - 1));
				}
			}
		} //ending for
		present = next;

		for (int xt, yt, k = 0; k < present.size(); k++)
		{
			xt = present[k].first;
			yt = present[k].second;

			if ((present[k].first == dest.first) && (present[k].second == dest.second))
			{
				loop = 0;
			}
		}

		if (loop)
		{
			len++;
		}
		else
		{
			route_len = len;
		}
	} //ending while

	total_route_length += route_len;

#if 1
	/********************BACKTRACKING*******************************************************/
	loop = 1;
	vector<pair<pair<int, int>, pair<int, int>>> decision_making;

	x_y_cord ele = dest;
	x_y_cord prev_coord;
	int previous_decision = 0;
	x_y_cord fr_case[4];
	int cases_num = 0;
	int x_cord;
	int y_cord;
	char turn[4];
	char prev_turn = 'T';
	int decision_turn = 0;
	int grid_selection = 1;
	int grid_route[4];
	int ele_dis;
	int tmp_dis;
	int turn_preference = 0;

	while (loop == 1)
	{
		cases_num = 0;
		path_coord.push_back(make_pair(ele, grid_selection));
		x_cord = ele.first;
		y_cord = ele.second;
		turn_preference = 0;
		if (grid_selection == 1)
			ele_dis = p_grid_route1[x_cord][y_cord].dist;
		else
			ele_dis = p_grid_route2[x_cord][y_cord].dist;

		/* CHECKING FOR SURROUNDING WHICH SATISFY THE CONDITION */
		if ((x_cord + 1) < row)
		{
			tmp_dis = p_grid_route2[x_cord + 1][y_cord].dist;
			if (tmp_dis == ele_dis - 1)
			{
				fr_case[cases_num] = make_pair(x_cord + 1, y_cord);
				turn[cases_num] = 'D';
				grid_route[cases_num] = 2;
				if (fr_case[cases_num] == src)
				{
					path_coord.push_back(make_pair(src, 2));
					break;
				}
				cases_num++;
			}
		}

		if ((x_cord - 1) >= 0)
		{
			tmp_dis = p_grid_route2[x_cord - 1][y_cord].dist;
			if (tmp_dis == ele_dis - 1)
			{
				fr_case[cases_num] = make_pair(x_cord - 1, y_cord);
				turn[cases_num] = 'T';
				grid_route[cases_num] = 2;
				if (fr_case[cases_num] == src)
				{
					path_coord.push_back(make_pair(src, 2));
					break;
				}
				cases_num++;
			}
		}

		if ((y_cord + 1) < col)
		{
			tmp_dis = p_grid_route1[x_cord][y_cord + 1].dist;
			if (tmp_dis == ele_dis - 1)
			{
				fr_case[cases_num] = make_pair(x_cord, y_cord + 1);
				turn[cases_num] = 'R';
				grid_route[cases_num] = 1;
				if (fr_case[cases_num] == src)
				{
					path_coord.push_back(make_pair(src, 1));
					break;
				}
				cases_num++;
			}
		}

		if ((y_cord - 1) >= 0)
		{
			tmp_dis = p_grid_route1[x_cord][y_cord - 1].dist;
			if (tmp_dis == ele_dis - 1)
			{
				fr_case[cases_num] = make_pair(x_cord, y_cord - 1);
				turn[cases_num] = 'L';
				grid_route[cases_num] = 1;
				if (fr_case[cases_num] == src)
				{
					path_coord.push_back(make_pair(src, 1));
					break;
				}
				cases_num++;
			}
		}

		/* CHECKING FOR TURNS */
		for (int i = 0; i < cases_num; i++)
		{
			if (decision_making.size() == 0)
			{
				grid_selection = grid_route[0];
				path_coord.push_back(make_pair(fr_case[0], grid_selection));
				decision_making.push_back(make_pair(ele, make_pair(grid_selection, cases_num)));
				prev_coord = ele;
				prev_turn = turn[0];
				ele = fr_case[0];
				break;
			}

			if ((turn[i] == prev_turn) || decision_turn != 0 || cases_num == 1)
			{
				if (decision_turn != 0)
				{
					grid_selection = grid_route[0];
					path_coord.push_back(make_pair(fr_case[0], grid_selection));
					prev_coord = ele;
					ele = fr_case[0];
					prev_turn = turn[0];
					decision_turn = 0;
				}
				else
				{
					grid_selection = grid_route[i];
					path_coord.push_back(make_pair(fr_case[i], grid_selection));
					if (cases_num > 1)
					{
						decision_making.push_back(make_pair(ele, make_pair(grid_selection, cases_num)));
					}
					prev_coord = ele;
					ele = fr_case[i];
					prev_turn = turn[i];
				}
				turn_preference = 1;
				break;
			}

			if (turn[i] != prev_turn && turn_preference == 0)
			{
				grid_selection = grid_route[i];
				path_coord.push_back(make_pair(fr_case[i], grid_selection));
				if (cases_num > 1)
				{
					decision_making.push_back(make_pair(ele, make_pair(grid_selection, cases_num)));
				}
				prev_coord = ele;
				ele = fr_case[i];
				prev_turn = turn[i];
				break;
			}
		}

		if (cases_num == 0)
		{
			if (grid_selection == 1)
				p_grid_route1[ele.first][ele.second].dist = 0;
			else
				p_grid_route2[ele.first][ele.second].dist = 0;

			pair<pair<int, int>, pair<int, int>> temp_dm;
			vector<pair<pair<int, int>, pair<int, int>>>::iterator it;
			pair<pair<int, int>, int> temp_coord_remover;
			vector<pair<pair<int, int>, int>>::iterator coord_remover;
			coord_remover = path_coord.end();
			it = decision_making.end();
			
			if (decision_making.size() <= 0) {
				return path_coord;
			}
				
			temp_dm = decision_making[decision_making.size() - 1];
			
			/* IF ALL POSSIBLE CASES IN LATEST DECISION SPOT ARE EXHAUSTED */
			while (temp_dm.first == ele)
			{
				it--;
				it = decision_making.erase(it);
				if (decision_making.size() <= 0)
				{
					return path_coord;
				}
				temp_dm = decision_making[decision_making.size() - 1];
			}

			ele = temp_dm.first;

			coord_remover--;
			temp_coord_remover = *coord_remover;
			while (temp_coord_remover.first != ele)
			{
				coord_remover = path_coord.erase(coord_remover);
				coord_remover--;
				temp_coord_remover = *coord_remover;
			}

			decision_turn = temp_dm.second.second;
			grid_selection = temp_dm.second.first;
			it--;
			decision_making.erase(it);
			decision_making.push_back(make_pair(ele, make_pair(grid_selection, decision_turn - 1)));
		}
	}

	int layer;
	int prev_layer;

	/* CHECKING ROUTE */
	for (int i = 0; i < path_coord.size(); i++)
	{
		if (i == 1)
			prev_layer = layer;

		if (path_coord[i].second == 1)
		{
			layer = 1;
			p_grid_route1[path_coord[i].first.first][path_coord[i].first.second].dist = -1;

		}
		else
		{
			layer = 2;
			p_grid_route2[path_coord[i].first.first][path_coord[i].first.second].dist = -1;
		}

		if (i != 0 && layer != prev_layer)
		{
			p_grid_route1[path_coord[i - 1].first.first][path_coord[i - 1].first.second].dist = -1;
			p_grid_route2[path_coord[i - 1].first.first][path_coord[i - 1].first.second].dist = -1;
			prev_layer = layer;
		}
	}
#endif

	for (int i = 0; i < path_coord.size(); i++)
	{
		if (i==0 && (path_coord.size()>1))
		{
			if(path_coord[i+1].second == 1)
				fprintf(output_fd, "<< metal1 >>\n");
			else
				fprintf(output_fd, "<< metal2 >>\n");

			fprintf(output_fd, "rect %d %d %d %d\n", path_coord[i].first.first, path_coord[i].first.second, path_coord[i].first.first + 1, path_coord[i].first.second + 1);

			if (path_coord[i + 3].second == 1)
			{
				fprintf(output_fd, "<< labels >>\n");
				fprintf(output_fd, "rlabel metal1 %d %d %d %d 0 Net #%d\n", path_coord[i+5].first.first, path_coord[i+5].first.second, path_coord[i+5].first.first + 1, path_coord[i+5].first.second + 1, net_value);
			}
			else
			{
				fprintf(output_fd, "<< labels >>\n");
				fprintf(output_fd, "rlabel metal2 %d %d %d %d 0 Net #%d\n", path_coord[i+5].first.first, path_coord[i+5].first.second, path_coord[i+5].first.first + 1, path_coord[i+5].first.second + 1, net_value);
			}
		}
		else if (p_grid_route1[path_coord[i].first.first][path_coord[i].first.second].dist == -1 && p_grid_route2[path_coord[i].first.first][path_coord[i].first.second].dist == -1)
		{
			fprintf(output_fd, "<< m2contact >>\n");
			fprintf(output_fd, "rect %d %d %d %d\n", path_coord[i].first.first, path_coord[i].first.second, path_coord[i].first.first + 1, path_coord[i].first.second + 1);
		}
		else if (path_coord[i].second == 1) {
			fprintf(output_fd, "<< metal1 >>\n");
			fprintf(output_fd, "rect %d %d %d %d\n", path_coord[i].first.first, path_coord[i].first.second, path_coord[i].first.first + 1, path_coord[i].first.second + 1);
		}
		else if (path_coord[i].second == 2) {
			fprintf(output_fd, "<< metal2 >>\n");
			fprintf(output_fd, "rect %d %d %d %d\n", path_coord[i].first.first, path_coord[i].first.second, path_coord[i].first.first + 1, path_coord[i].first.second + 1);
		}
	}


	if (path_coord[path_coord.size() - 1].second == 1)
	{
		fprintf(output_fd, "<< metal1 >>\n");
		fprintf(output_fd, "rect %d %d %d %d\n", src.first, src.second, src.first + 1, src.second + 1);

	}
	else
	{
		fprintf(output_fd, "<< metal2 >>\n");
		fprintf(output_fd, "rect %d %d %d %d\n", src.first, src.second, src.first + 1, src.second + 1);

	}

	if (path_coord[path_coord.size() - 3].second == 1)
	{
		fprintf(output_fd, "<< labels >>\n");
		fprintf(output_fd, "rlabel metal1 %d %d %d %d 0 Net #%d\n", path_coord[path_coord.size() - 5].first.first, path_coord[path_coord.size() - 5].first.second, path_coord[path_coord.size() - 5].first.first + 1, path_coord[path_coord.size() - 5].first.second + 1, net_value);
	}
	else
	{
		fprintf(output_fd, "<< labels >>\n");
		fprintf(output_fd, "rlabel metal2 %d %d %d %d 0 Net #%d\n", path_coord[path_coord.size() - 5].first.first, path_coord[path_coord.size() - 5].first.second, path_coord[path_coord.size() - 5].first.first + 1, path_coord[path_coord.size() - 5].first.second + 1, net_value);
	}
	return path_coord;
}

// argc = # of arguments passed to the program
// argv = pointers to the arguments, which are strings
int main(int argc, char* argv[])
{

	FILE* input_fd;        // a file stream for reading input
	FILE* output_fd;		// a file stream for writing the final partition and cutset

	// Argument parsing:
	// Verify correct argument count
	// Note: the system passes the full path of the
	// executable to the process as argv[0], hence
	// look for THREE arguments, even though only
	// using two.
	if (argc != 3) {

		// Print an error message to the error message output stream
		fprintf(stderr, "Expected file name as arguement, got %i\n", --argc);

		// Signal abnormal program termination
		return 1;
	}

	// Arg. parsing continues:
	// Find out if the specified input file exists:
	// if it does, open it
	if ((input_fd = fopen(argv[1], "r")) == NULL) {

		// if fopen returns NULL, then the file does not exist
		// print an error and quit.
		fprintf(stderr, "Specified input file \"%s\" does not exist.\n", argv[1]);
		return 1;
	}

#if 1
	// Arg. parsing continues:
	// Find out if the specified output file exists:
	// if it does, open it
	if ((output_fd = fopen(argv[2], "w")) == NULL) {

		// if fopen returns NULL, then the file does not exist
		// print an error and quit.
		fprintf(stderr, "Specified output file \"%s\" does not create.\n", argv[2]);
		fclose(input_fd);
		return 1;
	}
#endif

	char input_buffer[1024];   // an input buffer for the file stream

	// FILE *'s input_fd are reading for reading and writing,
	// respectively.
	// Now read the # nodes from the input file
	if (fgets(input_buffer, 1024, input_fd)) {
		// fgets() reads a line from the input file
		// the number of nodes is the first number sequence on the line
		// the atoi() function convertes the first number sequence to
		// an actual number.
		cells_num = atoi(input_buffer);
	}
	else {
		// fgets() can't read if there's nothing there to read!
		// signal and error, close the files, and quit.
		fprintf(stderr, "Structure of input file is invalid.\n");
		fclose(input_fd);
		fclose(output_fd);
		return 1;
	}

	// Read the # edges from the input file
	if (fgets(input_buffer, 1024, input_fd)) {
		// use atoi() to get a number from the string
		nets_num = atoi(input_buffer);
	}
	else {
		// like above, an error has arisin.  exit gracefully.
		fprintf(stderr, "Structure of input file is invalid.\n");
		fclose(input_fd);
		fclose(output_fd);
		return 1;
	}


	struct timeb start_t; // will hold algorithm start time
	struct timeb stop_t;  // will hold algorithm stop time  

	//Clock-in the start-time
	ftime(&start_t);

	placement_func(input_fd,output_fd);

	// Clock-in stop time
	ftime(&stop_t);

	// Prepare and print (to stdout, the screen) the running
	// time of the program
	int sec = stop_t.time - start_t.time;
	int milli_s = stop_t.millitm - start_t.millitm;
	if (milli_s < 0) {
		milli_s += 1000;
		sec--;
	}

	// print running time
	fprintf(stdout, "\n Running time of Placement and Routing (): %i seconds \t%i milli seconds.\n",
		sec, milli_s);

	// finish up execution:
	// tidy up by closing files
	fclose(input_fd);
	fclose(output_fd);

	cout <<endl << "ALL PATH ROUTED SUCCESFULLY ";
	cout << endl << "Total Route Length = " << total_route_length;
	cout << endl << "Final Bounding Box Sides -> Length = "<< sp_grid_row << " Breadth = " << sp_grid_col;
	cout << endl << "Final Bounding Box Area -> " << sp_grid_row * sp_grid_col;
	cout << endl << "No. of VIA's -> " << total_vias;
	cout << endl << "No. of Path Routed Succesfully -> " << nets_num;
	return 0;
}
