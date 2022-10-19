//    __author__ = "M.Satya Amarkant"
//    __version__ = "1.0"
//    __credits__ = ["M.SATYA AMARKANT"]
//    __license__ = "Private Domain :  Requires Approval for the use from M.SATYA AMARKANT"
//    __maintainer__ = "M.Satya Amarkant"
//    __contact__ = "amarkant23394@gmail.com"
//    __status__ = "Development"

#include <iostream>
#include <cstdio>
#include <vector>
#include <fstream>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <cstdlib>
#include <sys/timeb.h>  // for timeb struct
#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;
 
/********************* FUNCTION DECLARATIONS ****************************************/

static void KL_ALGO(FILE * input_fd,FILE * output_fd,int cells_num,int nets_num);
static void compute_gain_select_pair(vector<vector<int>> &weight_v,vector<int> &calc_D, vector<int> &par, int cells_num,int *max_g,int *Ai,int *Bi,int count);

/************************************************************************************/

static void compute_gain_select_pair(vector<vector<int>> &weight_v,vector<int> &calc_D, vector<int> &par, int cells_num,int *max_g,int *Ai,int *Bi,int count)
{
	vector<int> calc_DA(cells_num/2-count,0),cpy_calc_DA(cells_num/2-count,0);
	vector<int> calc_DB(cells_num/2-count,0),cpy_calc_DB(cells_num/2-count,0);
	vector<int> parA,parB; //(cells_num/2,0),parB(cells_num/2,0);	
	unordered_map<int,int> mapA,mapB;
	
	//cout<<"Entered the pair iteration loop of "<<count<<"\n";
	////cout<<"Displaying par"<<'\n';       //To display partition vector 
	//for(int i=0; i<par.size();i++)
	//{
	//	cout<<par[i]<<" ";
	//}
	//cout<<"\n";       //To display partitionsA,B and D values of each node along with its map

	int check=0;
	if(count!=0)  //To calculate updated D values after selecting first pair
	{
		for(int i=0; i<cells_num; i=i+2)  //Optimized the loop to reduce the no of iterations by half
		{
			check=0;
			if(par[i]>1)              //Skipping the calculation of Updated D for locked pairs
			{
				calc_D[i]=calc_D[i];
				check=1;
			}
			
			if(par[i+1]>1)            //Skipping the calculation of Updated D for locked pairs
			{
				calc_D[i+1]=calc_D[i+1];
				check=2;
				
				if(par[i]>1)
					check=3;
			}
			
			int sum_Cab_i=0;
			int sum_Cab_b_i=0;

			int sum_Cab_i_plus=0;
			int sum_Cab_b_i_plus=0;
			
			int size_of_loop;
			
			switch(check)            //Logic to select appropriate loop size for optimizing the calculations
			{
				case 0:
					size_of_loop = (weight_v[i].size() > weight_v[i+1].size())?(weight_v[i].size()) : (weight_v[i+1].size());
					break;
				case 1:
					size_of_loop = weight_v[i+1].size();
					break;
				case 2:
					size_of_loop = weight_v[i].size();
					break;
				default:
					size_of_loop = 0;
			}
		//cout<<"The value of loop and check is "<<size_of_loop<<" "<<check<<"\n";
			for(int wi=0;wi<size_of_loop;wi++)
			{
				if(check!=2 && (wi<weight_v[i+1].size()))
				{
					if((weight_v[i+1][wi] == *Ai) || (weight_v[i+1][wi] == *Bi))
					{
						if(weight_v[i+1][wi] == *Bi)
							sum_Cab_b_i_plus++;
						if(weight_v[i+1][wi] == *Ai)
							sum_Cab_i_plus++;
					}
				}

				if(check!=1 && (wi<weight_v[i].size()))
				{				
					if((weight_v[i][wi] == *Ai) || (weight_v[i][wi] == *Bi))
					{
						if(weight_v[i][wi] == *Bi)
							sum_Cab_b_i++;
						if(weight_v[i][wi] == *Ai)
							sum_Cab_i++;
					}
				}
			}
			
			int diff_weight = par[i]?(sum_Cab_i-sum_Cab_b_i):(sum_Cab_b_i-sum_Cab_i);
			calc_D[i]=calc_D[i]+2*diff_weight; //(abs(weight_v[i][*Ai]))-2*(abs(weight_v[i][*Bi]));
			
			diff_weight = par[i+1]?(sum_Cab_i_plus-sum_Cab_b_i_plus):(sum_Cab_b_i_plus-sum_Cab_i_plus);
			calc_D[i+1]=calc_D[i+1]+2*diff_weight; //(abs(weight_v[i][*Ai]))-2*(abs(weight_v[i][*Bi]));
			
		}		
	}
	
	//cout<<"Creating the maps and temp partition vectors\n"	;
	for(int a=0,b=0,i=0;i<par.size();i++)
	{
		if(par[i]>1)
		{ 
			continue;
		}
		else
		{
			if(!par[i])
			{
				calc_DB[b] = calc_D[i];
				parB.push_back(i);
				mapB.insert(make_pair(i,calc_DB[b]));   //Creating an unordered map to store the indices of partition B nodes
				b++;
			}
			else
			{
				calc_DA[a] = calc_D[i];
				parA.push_back(i);
				mapA.insert(make_pair(i,calc_DA[a]));   //Creating an unordered map to store the indices of partition A nodes
				a++;
			}
		}
	}
	
	//cout<<"Sorting the vector DA\n"	;
	sort(calc_DA.begin(), calc_DA.end(), greater<int>()); 
	//cout<<"Sorting the vector DB\n"	;
	sort(calc_DB.begin(), calc_DB.end(), greater<int>());
	//cout<<"Finding the unique values in the vector DA\n";
	vector<int>::iterator ip;    
        ip=unique_copy(calc_DA.begin(), calc_DA.end(),cpy_calc_DA.begin());  //To optimize the vector copy using the unique_copy function 
	cpy_calc_DA.resize(std::distance(cpy_calc_DA.begin(), ip));          //Removing the repeated elements
	//cout<<"Finding the unique values in the vector DB\n";
        ip=unique_copy(calc_DB.begin(), calc_DB.end(),cpy_calc_DB.begin());  //To optimize the vector copy using the unique_copy function 
	cpy_calc_DB.resize(std::distance(cpy_calc_DB.begin(), ip));          //Removing the repeated elements

	//cout<<"Copying the vector DA\n"	;
	//cpy_calc_DA = calc_DA;   //Creating a copy of DA to remove extra keys found on the sorted vector
	//cout<<"Copying the vector DB\n"	;
	//cpy_calc_DB = calc_DB;   //Creating a copy of DB to remove extra keys found on the sorted vector
	//cout<<"Finding the unique values in the vector DA\n"	;
	//auto last = unique(cpy_calc_DB.begin(), cpy_calc_DB.end());  //To find unique values in the sorted array of DA 
	//cout<<"Erasing the duplicate values in the vector DA\n"	;
	//cpy_calc_DB.erase(last,cpy_calc_DB.end());
	//cout<<"Finding the unique values in the vector DB\n"	;
	//last = unique(cpy_calc_DA.begin(), cpy_calc_DA.end());    //To find unique values in the sorted array of DB
	//cout<<"Erasing the duplicate values in the vector DB\n"	;
	//cpy_calc_DA.erase(last,cpy_calc_DA.end());
	
	vector<int> sort_DAKeys,sort_DBKeys;
	
	//cout<<"Finding the unique keys in the vector DA\n"	;
	for(int i=0;i<cpy_calc_DA.size();i++)
	{
		for (const auto& element : mapA)
		   if (element.second == cpy_calc_DA[i])
        		sort_DAKeys.push_back(element.first);
	}
	
	//cout<<"Finding the unique keys in the vector DB\n"	;
	for(int i=0;i<cpy_calc_DB.size();i++)
	{
		for (const auto& element : mapB)
		   if (element.second == cpy_calc_DB[i])
        		sort_DBKeys.push_back(element.first);
	}
    
    //Calculating total g's with optimization logic implemented 
	if(sort_DAKeys.size() == 1)       //Calculating the pairwise gain for the last unlocked pair
	{
		int a=0,b=0,g_temp=0,Cab;
		a=sort_DAKeys[0];
		b=sort_DBKeys[0];
		int sum_Cab=0;
		
		for(int wi=0;wi<weight_v[a].size();wi++)
		{
			if(weight_v[a][wi] == b)
			sum_Cab++;
		}
		
		g_temp = (calc_D[a] + calc_D[b] - 2 * sum_Cab); // (abs(weight_v[a][b])));
		*Ai = a;
		*Bi = b;
		*max_g = g_temp;
		//cout<<"In if block Max value and pairs are"<<*max_g<<" "<<*Ai<<" "<<*Bi<<endl;
	}	
	else
	{
	for(int g_temp=0,d_temp=0,bi_temp=0,a=0,b=0,i=0; i<sort_DAKeys.size();i++)    //Calculating max pairwise gain for unlocked pairs
	{
		a=sort_DAKeys[i];
		for(int sum_Cab,j=0;j<sort_DBKeys.size();j++)
		{
			sum_Cab=0;
			b=sort_DBKeys[j];
			for(int wi=0;wi<weight_v[a].size();wi++)
			{
				if(weight_v[a][wi] == b)
				sum_Cab++;
			}
			g_temp = (calc_D[a] + calc_D[b] - 2 * sum_Cab); //(abs(weight_v[a][b])));
			bi_temp = sort_DBKeys[j+1];
			d_temp = calc_D[a] + calc_D[bi_temp];
//			//cout<<"Value ofa,b,calc_D[a] ,calc_D[a],bi_temp and d_temp and g_temp is "<<a<<"  "<<b<<"  "<<calc_D[a]<<"  "<<calc_D[b]<<"  "<<bi_temp<<"  "<<d_temp<<"  "<<g_temp<<"\t";
			if(d_temp <= g_temp)
			{
				*Ai = a;
				*Bi = b;
				*max_g = g_temp;
				goto done;
			}
		}
		//cout<<endl;
	}
	done:
	int x=1;
	//cout<<" In else Max value and pairs are"<<*max_g<<" "<<*Ai<<" "<<*Bi<<endl;
}
}


static void KL_ALGO(FILE * input_fd,FILE * output_fd,int cells_num,int nets_num)
{
	int fpos=0;             //To find the position of file pointer to repeat the reading of the file
	char file_buffer[256];	//a buffer for reading input
	char *trunc_str;		//token for input parsing
	int f_tok;				//for parsing first read node
	int s_tok;				//for parsing second read node
	int temp_nets=0;
	int pass_count=0;
	vector<int> calc_D(cells_num,0);
	vector<int> gain_queue(cells_num/2,0),parA(cells_num/2,0),parB(cells_num/2,0);
	vector<int> par,par_last; 	
	int max_g,Ai,Bi,iter_count=0,max=0,peak=0;                           
	fpos=ftell(input_fd);        //To read the Benchmark files at correct position to calculate the cutset size

	for (int i = 0; i < cells_num; i++)
	{
		if(i%2 == 0)
		{
			par.push_back(1); //Setting 1 for Partition A
		}
		else
		{
			par.push_back(0); //Setting 0 for Partition B
		}
	}
	
	int flag_pass=0;
	vector<vector<int>> conc_vec(cells_num);//, vector<int> (cells_num, 0));
	temp_nets = 0;
	fseek(input_fd,fpos,SEEK_SET);
/*	cout<<"Started reading the file to create the weight vector \n";  //Initial 2-D matrix for representing weights
	while(fgets(file_buffer,256,input_fd)) {
		if(temp_nets>=nets_num)
		{
			////cout<<"Breaking the file reading loop\n";
			break;
		}
		
		trunc_str = strtok(file_buffer," \t\n");	//reading first token
		if(trunc_str != NULL) f_tok = atoi(trunc_str);
		else break;					//on error break out
		
		trunc_str = strtok(NULL," \t\n");		//reading second token
		if(trunc_str != NULL) s_tok = atoi(trunc_str);
		else break;
		if((par[f_tok-1] == 0 && par[s_tok-1] == 0) || (par[f_tok-1] == 1 && par[s_tok-1] == 1))
		{
			conc_vec[f_tok-1][s_tok-1]-=1;
			conc_vec[s_tok-1][f_tok-1]-=1;
		}
		else if((par[f_tok-1] == 0 && par[s_tok-1] == 1) || (par[f_tok-1] == 1 && par[s_tok-1] == 0))
		{
			conc_vec[f_tok-1][s_tok-1]+=1;
			conc_vec[s_tok-1][f_tok-1]+=1;
		}
		temp_nets++;
	}
	cout<<"Completed reading the file to create the weight vector \n";
*/	
////Trying optmization to remove multiple reads from file and clearing the 2D vector
	temp_nets = 0;
	fseek(input_fd,fpos,SEEK_SET);
	cout<<"Started reading the file to create the weight vector \n";
	while(fgets(file_buffer,256,input_fd)) {
		if(temp_nets>=nets_num)
		{
			////cout<<"Breaking the file reading loop\n";
			break;
		}
		
		trunc_str = strtok(file_buffer," \t\n");	//reading first token
		if(trunc_str != NULL) f_tok = atoi(trunc_str);
		else break;					//on error break out
		
		trunc_str = strtok(NULL," \t\n");		//reading second token
		if(trunc_str != NULL) s_tok = atoi(trunc_str);
		else break;
		conc_vec[f_tok-1].push_back(s_tok-1);  //Modified the weighted vector with adjacency list using vector of vectors because of memory issues
		conc_vec[s_tok-1].push_back(f_tok-1);

		temp_nets++;
	}

do
{
	max=0;
	par_last=par;
	//cout<<"Displaying initial par"<<'\n';       //To display initial partition
//	for(int i=0; i<par.size();i++)
//	{
//		cout<<par[i]<<" ";
//	}
	//cout<<"\n";       
	iter_count = 0;
	
	//cout<<"Completed reading the file to create the weight vector \n";
	for(int sum,i=0; i<conc_vec.size(); i++)       //Calculating the initial D values for all the nodes
	{
		sum=0;
		if(par[i])
		{
			for(int j=0; j<conc_vec[i].size();j++)
			{
				if(par[conc_vec[i][j]])
					sum--;
				else
					sum++;
			}

		}
		else
		{
			for(int j=0; j<conc_vec[i].size();j++)
			{
				if(par[conc_vec[i][j]])
					sum++;
				else
					sum--;
			}
		}
		calc_D[i] = sum;
	}
//		for(int i=0;i<conc_vec.size();i++)            //To display conc_vec
//	{
//		for(int j=0;j<conc_vec[i].size();j++)
//		{
//		  cout<<conc_vec[i][j]<<" ";
//		}
//		cout<<endl;
//	}
//	cout<<'\n'<<'\n';	
	cout<<"Started the process of Calculating the pairwise gains for a new partition set\n";
	int display_count=5000;
    
	for (int i=0;i<cells_num/2;i++)	
	{
        compute_gain_select_pair(conc_vec,calc_D,par,cells_num,&max_g,&Ai,&Bi,iter_count);
		par[Ai] = 2;   //Locking the exchanged pairs
		par[Bi] = 2;   //Locking the exchanged pairs
		gain_queue[iter_count] = max_g;  //Inserting the pairwise gain into gain-queue
		parA[iter_count] = Ai;    //Storing the exchanged pairs into respective partitions
		parB[iter_count] = Bi;    //Storing the exchanged pairs into respective partitions
		iter_count++;

		if(i==display_count)
		{
			cout<<"Calculated the pairwise gains for "<<display_count<<" pairs\n";
			display_count+=5000;
		}
	}
	
	max=gain_queue[0];
	//fprintf(output_fd, "\nPrinting the gain queues for %d pass_count\n",pass_count);

	for(int sum=0,i=0; i<gain_queue.size();i++)        //Calculating the cummulative max gain value and also the subqueue length of the gain queue
	{
		//fprintf(output_fd, "%d %d\n",gain_queue[i],i);
		sum+=gain_queue[i];
		//max=sum;
		if(sum>=max)
		{
			max = sum;
			peak = i;
		}
	}
	//fprintf(output_fd, "\n");
		
	//cout<<"Max and peak values are "<<max<<"  "<<peak<<"  ";
	//cout<<endl;
	if(max>0)
	{
		for(int a,i=0; i<parA.size();i++)      //Retaining the unexchanged pair values in partitionA
		{
			a=parA[i];
			par[a]=1;
		}
		for(int a,i=0; i<parB.size();i++)      //Retaining the unexchanged pair values in partitionA  
		{
			a=parB[i];
			par[a]=0;
		}
		for(int a=0,b=0,i=0; i<peak;i++)       //Exchanging all the selected pairs for the current pass
		{
			a=parA[i];
			b=parB[i];
			par[a]=0;
			par[b]=1;
		}
	}
	pass_count++;
	
	int a=0,b=0;
	if(max>0 && peak==0)     //To Exchange the final selected pair for the last iteration(peak=0 => finalpair as per vector indexing) 
	{
		a=parA[0];
		b=parB[0];
		par[a]=0;
		par[b]=1;
	}
	temp_nets = 0;
	int cut_size=0;
	fseek(input_fd,fpos,SEEK_SET);  //To read the cut_set size of the updated partition
	while(fgets(file_buffer,256,input_fd)) {
		if(temp_nets>=nets_num)
			break;
		
		trunc_str = strtok(file_buffer," \t\n");	//reading first token
		if(trunc_str != NULL) f_tok = atoi(trunc_str);
		else break;					//on error break out
		
		trunc_str = strtok(NULL," \t\n");		//reading second token
		if(trunc_str != NULL) s_tok = atoi(trunc_str);
		else break;
		
		if((par_last[f_tok-1] == 0 && par_last[s_tok-1] == 1) || (par_last[f_tok-1] == 1 && par_last[s_tok-1] == 0))
		{
			cut_size++;  //Incrementing cutsize for external connections
		}
		temp_nets++;
	}
	if(max>0)
	{
	cout<<"Optimal cut size is "<<cut_size<<", max cummulative gain is "<<max<<" and subqueue length is "<<peak+1<<" for the "<<pass_count<<" pass iteration\n";
	//fprintf(output_fd, "%d %d %d %d\n",cut_size,max,(peak+1),pass_count);
	}
	else
	{
	cout<<"Optimal cut size is "<<cut_size<<" max cummulative gain is "<<max<<" and subqueue length is 0 for the "<<pass_count<<" pass iteration\n";
	//fprintf(output_fd, "%d %d %d %d\n",cut_size,max,(peak+1),pass_count);
	}

//	cout<<"Executed the "<<pass_count<<" pass iteration\n";
//	conc_vec.clear();
//	for(int i=0;i<conc_vec.size();i++)            //To print weight_v
//	{
//	  conc_vec[i].clear();//[j]=0;
//	}
	
	}while(max>0);

	temp_nets = 0;
	int cut_size=0;
	fseek(input_fd,fpos,SEEK_SET);  //To read the cut_set size
	while(fgets(file_buffer,256,input_fd)) {
		if(temp_nets>=nets_num)
			break;
		
		trunc_str = strtok(file_buffer," \t\n");	//reading first token
		if(trunc_str != NULL) f_tok = atoi(trunc_str);
		else break;					//on error break out
		
		trunc_str = strtok(NULL," \t\n");		//reading second token
		if(trunc_str != NULL) s_tok = atoi(trunc_str);
		else break;
		
		if((par_last[f_tok-1] == 0 && par_last[s_tok-1] == 1) || (par_last[f_tok-1] == 1 && par_last[s_tok-1] == 0))
		{
			cut_size++;
		}
		temp_nets++;
	}
	cout<<"Optimal cut size is "<<cut_size<<"\n";
	fprintf(output_fd, "Optimal cut size is %d \n",cut_size);
	
	cout<<"Displaying Final partitionA"<<'\n';       
	fprintf(output_fd, "Displaying Final partitionA\n");
	for(int i=0; i<par_last.size();i++)
	{
		if(par_last[i])
		{
		cout<<i+1<<" ";
		fprintf(output_fd, "%d ",(i+1));
		}
	}
	fprintf(output_fd, "\n");
	cout<<endl<<endl;
	cout<<"Displaying Final partitionB"<<'\n';       
	fprintf(output_fd, "Displaying Final partitionB\n");
	for(int i=0; i<par_last.size();i++)
	{
		if(!(par_last[i]))
		{
		cout<<i+1<<" ";
		fprintf(output_fd, "%d ",(i+1));
		}
	}
}

// C/C++ conventions:
// argc = # of arguments passed to the program
// argv = pointers to the arguments, which are strings
int main(int argc, char * argv[]) {
	auto start = high_resolution_clock::now();
	

	FILE * input_fd;        // a file stream for reading input
	FILE * output_fd;        // a file stream for reading input

	// Argument parsing:
	// Verify correct argument count
	// Note: the system passes the full path of the
	// executable to the process as argv[0], hence
	// look for THREE arguments, even though only
	// using two.
	if(argc!=3) {

		// Print an error message to the error message output stream
		fprintf(stderr,"Expected file name as arguement, got %i\n",--argc);

		// Signal abnormal program termination
		return 1;
	}

	// Arg. parsing continues:
	// Find out if the specified input file exists:
	// if it does, open it
	if((input_fd = fopen(argv[1],"r"))==NULL) {
		
		// if fopen returns NULL, then the file does not exist
		// print an error and quit.
		fprintf(stderr,"Specified input file \"%s\" does not exist.\n",argv[1]);
		return 1;
	}
	// Arg. parsing continues:
	// Find out if the specified output file exists:
	// if it does, open it
	if ((output_fd = fopen(argv[2], "w")) == NULL) {
	
	// if fopen returns NULL, then the file does not exist
	// print an error and quit.
	fprintf(stderr, "Specified output file \"%s\" does not create.\n", argv[2]);
	fclose(output_fd);
	return 1;
	}	
	char input_buffer[1024];   // an input buffer for the file stream

	int cells_num;           // number of nodes
	
	// FILE *'s input_fd are reading for reading and writing,
	// respectively.
	// Now read the # nodes from the input file
	if(fgets(input_buffer,1024,input_fd)) {
		// fgets() reads a line from the input file
		// the number of nodes is the first number sequence on the line
		// the atoi() function convertes the first number sequence to
		// an actual number.
		cells_num = atoi(input_buffer);
	} else {
		// fgets() can't read if there's nothing there to read!
		// signal and error, close the files, and quit.
		fprintf(stderr,"Structure of input file is invalid.\n");
		fclose(input_fd);
		return 1;
	}

	int nets_num;           // number of nets
	
	// Read the # edges from the input file
	if(fgets(input_buffer,1024,input_fd)) {
		// use atoi() to get a number from the string
		nets_num = atoi(input_buffer);
	} else {
		// like above, an error has arisin.  exit gracefully.
		fprintf(stderr,"Structure of input file is invalid.\n");
		fclose(input_fd);
		return 1;
	}

	cout<<"Call the function, KL_ALGO(), to start\n"<<'\n';	
	KL_ALGO(input_fd,output_fd,cells_num,nets_num);

	fclose(input_fd);
	fclose(output_fd);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<seconds>(stop - start);
	cout<<"\nThe time taken to execute is "<< duration.count()<<endl;
	return 0;
}

