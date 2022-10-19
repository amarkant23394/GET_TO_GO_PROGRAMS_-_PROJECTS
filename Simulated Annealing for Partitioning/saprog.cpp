//    __author__ = "M.Satya Amarkant"
//    __version__ = "1.0"
//    __credits__ = ["M.SATYA AMARKANT"]
//    __license__ = "Private Domain :  Requires Approval for the use from M.SATYA AMARKANT"
//    __maintainer__ = "M.Satya Amarkant"
//    __contact__ = "amarkant23394@gmail.com"
//    __status__ = "Development"

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <time.h>
#include <sys/timeb.h>  // for timeb struct
#include <random>

using namespace std;

#define _CRT_SECURE_NO_WARNINGS 1

/********************** GLOBAL and STATIC VARIABLES ********************************************/
default_random_engine generator;
double alpha = 0.99; // COOLING RATIO DEFAULT FOR NODES 500000
double beta = 1.0;
double tf = 1.0;
double t = 390000; //TEMPERATURE DEFAULT for nodes 500000
double m = 350000; //M LOOP DEFAULT for nodes 500000
static vector<bool> partition_vec;	//Binary vector representing partition
int first_node = 0;
int second_node = 0;
int best_cost = 0;
int cells_num=0;           // number of nodes
/************************************************************************************/


/********************* FUNCTION DECLARATIONS ****************************************/
static int SimmulatedAnnealing(FILE* input_fd, int cells_num, int nets_num);
static unsigned int random_within_range(unsigned int low, unsigned int high);
static double random_num_gen();
static int cost_calculator(vector<vector<int>>& conc_vec);
static void metropolis_func(vector<vector<int>>& conc_vec, double t, double m);
static int perturb_move_func(vector<vector<int>>& conc_vec);
static int output_partition_to_file(FILE* output_fd);
/************************************************************************************/

//To calculate the cost of the solution
//Parameter - the connection vector
static int cost_calculator(vector<vector<int>>& conc_vec)
{
	int cost = 0;
	for (int i = 0; i < conc_vec.size(); i++)
	{
		int partition_stance = partition_vec[i];
		for (int j = 0; j < conc_vec[i].size(); j++)
		{
			if (partition_stance != partition_vec[conc_vec[i][j]])
				cost++;
		}
	}
	return (cost / 2);
}

// To generate random integer within the given range
// low = lower range
// high = higher range
static unsigned int random_within_range(unsigned int low, unsigned int high) {
	uniform_int_distribution<int> distribution(low, high);
	return distribution(generator);
};

//To generate random number between 0-1
static double random_num_gen() {
	double ret_val;
	ret_val = 0.001 * random_within_range(0, 999);
	return ret_val;
}

static int perturb_move_func(vector<vector<int>>& conc_vec)
{
	int one_node = random_within_range(0, partition_vec.size() - 1);			//selecting first node from a partition

	// selecting another node which is of different partition
	int sec_node;
	do {
		sec_node = random_within_range(0, partition_vec.size() - 1);
	} while (partition_vec[one_node] == partition_vec[sec_node]);

	first_node = one_node;
	second_node = sec_node;

	/* PREVIOUS SWAPPING */
	//swapping the external and internal connections of the node respectively
	//swap(f_ex_conc_vec[one_node], f_in_conc_vec[one_node]);
	//swap(f_ex_conc_vec[sec_node], f_in_conc_vec[sec_node]);

	int cost = 0;
	int gain_conectivity = 0;
	//changing the external and internal count of the swapped nodes w.r.t others

/* COMMENTED BECAUSE THIS LOOP WAS NOT GIVING OPTIMZED SPEED */
#if 0
	/* CALCULATING GAIN FOR FIRST NODE*/
	for (unsigned int j = 0; j < conc_vec[first_node].size(); j++)
	{
		if (partition_vec[first_node] == partition_vec[conc_vec[first_node][j]])
			cost--;
		else
			cost++;

		if (conc_vec[first_node][j] == second_node)
			gain_conectivity++;
	}

	/* CALCULATING GAIN FOR SECOND NODE*/
	for (unsigned int j = 0; j < conc_vec[second_node].size(); j++)
	{
		if (partition_vec[second_node] == partition_vec[conc_vec[second_node][j]])
			cost--;
		else
			cost++;
	}
#endif

	unsigned int loop_size = (conc_vec[first_node].size() > conc_vec[second_node].size()) ? conc_vec[first_node].size() : conc_vec[second_node].size();

	/* CALCULATING GAIN FOR FIRST NODE*/
	for (unsigned int j = 0; j < loop_size; j++)
	{
		if (j < conc_vec[first_node].size()) {
			if (partition_vec[first_node] == partition_vec[conc_vec[first_node][j]])
				cost--;
			else
				cost++;

			if (conc_vec[first_node][j] == second_node)
				gain_conectivity++;
		}

		if (j < conc_vec[second_node].size()) {
			if (partition_vec[second_node] == partition_vec[conc_vec[second_node][j]])
				cost--;
			else
				cost++;
		}
	}

	cost = cost - 2 * gain_conectivity;
	cost = best_cost - cost;

	return cost;
}

static void metropolis_func(vector<vector<int>>& conc_vec, double t, double m)
{
	int new_cost = 0;

	double k = 1.38064852e-23;		//botlz equation
	int diff_cost;	//h

	//Metropolic loop 1 to M
	for (int i = 1; i <= m; i++)
	{
		//cout << endl << " M LOOP " << new_cost <<endl;
		new_cost = perturb_move_func(conc_vec);		//move function for swapping a pair 

		diff_cost = new_cost - best_cost;		//calculating h 

		//if the new partition costs less then it is selected and loop is end
		if (diff_cost < 0) {
			swap(partition_vec[first_node], partition_vec[second_node]);
			best_cost = new_cost;
		}
		else {
			double rand_num = random_num_gen();

			//if new partition costs more then it is selected based on a exponential random inequality
			if (rand_num < exp(((double)-diff_cost) / (k * t)))
			{
				swap(partition_vec[first_node], partition_vec[second_node]);
				best_cost = new_cost;
			}
		}
	}
}

static int SimmulatedAnnealing(FILE* input_fd, int cells_num, int nets_num)
{
#if 1

	//initial partition is done over here
	//random initial solution
	//even odd partition
	for (int i = 0; i < cells_num; i++)
		if (i % 2 == 0)
			partition_vec.push_back(1);
		else
			partition_vec.push_back(0);

#endif


#if 1
	//Netlist is read from file and vector is filled accordingly
	//file = input_fd = file buffer
	//vector = the connection between nodes = diagonnaly symmetric matrix

	char file_buffer[256];	//a buffer for reading input
	char* trunc_str;		//token for input parsing
	int f_tok;				//for parsing first read node
	int s_tok;				//for parsing second read node
	int temp_nets = 0;
	vector<vector<int>> conc_vec;
	conc_vec.resize(cells_num);			//can't declare with variable so resizing it //reference stack overflow

	while (fgets(file_buffer, 256, input_fd)) {
		if (temp_nets >= nets_num)
			break;

		trunc_str = strtok(file_buffer, " \t\n");	//reading first token
		if (trunc_str != NULL) f_tok = atoi(trunc_str);
		else break;									//on error break out

		trunc_str = strtok(NULL, " \t\n");	//reading second token
		if (trunc_str != NULL) s_tok = atoi(trunc_str);
		else break;

		/* CORRECTION */
		//cout << "FIRST NODE : " << f_tok << " SECOND NODE : " << s_tok << endl;

		conc_vec[f_tok - 1].push_back(s_tok - 1);			//first node connection
		conc_vec[s_tok - 1].push_back(f_tok - 1);			//second node connection

		temp_nets++;
	}

#if 0
	/* CORRECTION FOR ALLOCATION OF MARTRIX*/
	for (int i = 0; i < conc_vec.size(); i++)
	{
		for (int j = 0; j < conc_vec[i].size(); j++)
		{
			cout << conc_vec[i][j] << " ";
		}
		cout << endl;
	}
#endif
#endif

	//Initial solution is provided
	//Initial T value is provided
	//Initial M value is provided

	best_cost = cost_calculator(conc_vec);		//considering the initial design cost as the best cost yet

#if 1
	//SimmulatedAnnealing Procedure is started here 
	do {
		metropolis_func(conc_vec, t, m);
		t = t * alpha;	//decrease T
		m = m * beta;	//increase M
        cout<<endl<<"The temperature is "<<t;
	} while (t > tf);
#endif

	//best_cost = cost_calculator(conc_vec);
	cout << endl << "Cost : " << best_cost << endl;

	return 0;
}

//Writes the final cost and partition into a file
//Arguement - File descriptor for the output file
static int output_partition_to_file(FILE* output_fd)
{
	//FILE* output_fd;        // file descriptor to write the final partition of the output to the file

	fprintf(output_fd, "The sub optimal cut set is  \"%d\" \n", best_cost);
	fprintf(output_fd, "\n Nodes in partition A : ");
	for (int i = 0; i < cells_num; ++i)
	{
		if (partition_vec[i] == 0)
			fprintf(output_fd, " %d", (i + 1));
	}

	fprintf(output_fd, "\n Nodes in partition B : ");
	for (int i = 0; i < cells_num; ++i)
	{
		if (partition_vec[i] == 1)
			fprintf(output_fd, " %d", (i + 1));
	}

	return 0;
}

// argc = # of arguments passed to the program
// argv = pointers to the arguments, which are strings
int main(int argc, char* argv[]) {

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

	int nets_num;           // number of nets

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

	/* CORRECTION */
	//srand(time(NULL)); //previous usage //now using uniform distribution for better randomality

	struct timeb start_t; // will hold algorithm start time
	struct timeb stop_t;  // will hold algorithm stop time  

	//Clock-in the start-time
	ftime(&start_t);

    cout<<endl<<"************THE SIMMULATED ANNEALING PROCESS STARTED*************************"<<endl;

	// Call the workhorse function, SimmulatedAnnealing(), to start
	// the partitioning process.
	if (SimmulatedAnnealing(input_fd, cells_num, nets_num) < 0) {
		// signal an error on stderr
		fprintf(stderr, "SimmulatedAnnealing() algorithm failed.\n");

		// tidy up and quit
		fclose(input_fd);
		fclose(output_fd);
		return 1;
	}

	// Clock-in stop time
	ftime(&stop_t);

	output_partition_to_file(output_fd);

	// Prepare and print (to stdout, the screen) the running
	// time of the program
	int sec = stop_t.time - start_t.time;
	int milli_s = stop_t.millitm - start_t.millitm;
	if (milli_s < 0) {
		milli_s += 1000;
		sec--;
	}

	// print running time
	fprintf(stdout, "Running time of Simmulated Annealing(): %i seconds \t%i milli seconds.\n",
		sec, milli_s);

	// finish up execution:
	// tidy up by closing files
	fclose(input_fd);
	fclose(output_fd);

	return 0;
}
