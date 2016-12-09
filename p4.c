/*
 
 Dan Stowe: ds356671
 Andrew Caldwell: ac289774
 
 Discussion Session: Friday 1:40 PM
 
 For this project we both split the workload 50/50. Dan worked on the functions that held the
 identifiers in the array while Andrew worked on processing the MAL statements. With testing, Dan debugged the make_CRT
 function and Andrew tested the print_listing function.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/*
 Prototypes (mentions who wrote which function)
 */
void print_listing(FILE *in,FILE *out); // Written by Dan Stowe
void make_CRT(FILE *in, FILE *out);     // Written by Dan Stowe
char *remove_comments(char *line);       // Written by Andrew Caldwell
char *remove_label(char *line);         // Written by Andrew Caldwell
int check_break(char *line,int length);     // Written by Andrew Caldwell
int check_used(char *line,char *needle);    // Written by Andrew Caldwell


/*
 Main function that takes in arguments from the user. Checks for number of arguments and
 valid input to read from and write to files.
 
 - Written by Andrew Caldwell
 */
int main(int argc, char *argv[])
{
    FILE *input_file, *output_file;
    if(argc != 4)             // Check for correct number of arguments
    {
        fprintf(stderr,"Invalid arguments\n");
        exit(1);
    }
    if(argv[1][0]!='-')
    {
        fprintf(stderr,"First argument must be a flag (-l/c/b)\n");
        exit(1);
    }
    
    input_file=fopen(argv[2],"r");
    if(input_file == NULL)      // Check if input file exists
    {
        fprintf(stderr,"Can`t open input file\n");
        exit(1);
    }
    output_file=fopen(argv[3],"w"); // Check if output file exists
    if(output_file==NULL)
    {
        fprintf(stderr,"Can`t open output file\n");
        exit(1);
    }
    switch(argv[1][1])  // Determine what to do based on the flag provided
    {
        case 'l':
            print_listing(input_file,output_file);
            break;
        case 'c':
            make_CRT(input_file,output_file);
            break;
        case 'b':
            print_listing(input_file,output_file);
            make_CRT(input_file,output_file);
            break;
    }
    fclose(input_file);     // Close input file
    fclose(output_file);    // Close output file
    return 0;
}   // end main


/*
 returns 1 if the line is empty
 otherwise returns 0
 */
int check_break(char *line,int length)
{
    int i;
    for(i=0; i<length;i++)  // cycle goes through all chars in string
    {
        if (!isspace(line[i])) // isspace()-library function   it checks if char is whitespace or no.
            return 0;
    }
    return 1;
}

/*
 Removes comments chars from a line if the line contains a '#'
 returns the line
 */
char *remove_comments(char *line)
{
    char *comment;
    // char *without_comments;
    if((comment=strstr(line,"#"))!=NULL)
    {
        line[comment-line]='\0';
    }
    return line;
}

/*
 Removes labels from line if line contains ':'
 returns the line
 */
char *remove_label(char *line)
{
    char *newline;
    if((newline=strstr(line,":"))!=NULL)
        line=newline;
    return line;
}


void print_listing(FILE *in,FILE *out)
{
    char *tmp_line = NULL;  //current line of the file will be temporary saved in this variable
    int read;               //number of chars in tmp_line
    int linecount=1;           // counter for the lines
    size_t len = 0;
    
    while((read = getline(&tmp_line,&len,in)) != -1) // cycle works until we dont read any char from the getline() function
    {
        if(!check_break(tmp_line,read))       // checking if the line is not empty
    {
        fprintf(out,"%d\t",linecount++);      // printing the number of line and increasing counter
    }
        fprintf(out,"%s",tmp_line);         // printing the line in the file
    }
}

/*
function checks if the "needle" identifier is used in "line"
returns 0 if not
1 if used
 */
int check_used(char *line,char *needle)
{
    char *identifier=strstr(remove_label(line),needle);
    if(identifier == NULL)
        return 0;
    
    int id = (int)identifier[strlen(needle)]; //get the next char int value
    
    if((id >= 48 && id <= 57) || (id >= 97 && id <= 122) || (id >= 65 && id <= 90))     // 48-57 is numbers  97-122 lower case 65-90 upper case letters
        return 0;
    return 1;
}


/*
 Create a cross reference table that shows where an identifier is defined and used
 in the MAL program.
 */
void make_CRT(FILE *in, FILE *out)
{
    rewind(in);     // put pointer to the start of input file
    
    char *tmp_line=NULL;    //current line of the file will be temporary saved in this variable
    int linecount=1;        // counter for the lines
    int read;               //number of chars in tmp_line (needed for check_break function)
    size_t len = 0;
    char identifiers[100][11];  // array of identifiers
    int identif_counter=0;      // current identifier
    int defined[100];       // here is line number where each identifier is defined
    char used[100][10];     // here lines where identifier is used are holding
    
    fprintf(out,"\nCross Reference Table\n\nIdentifier\tDefinition\tUse\n\n");
    
    while((read=getline(&tmp_line,&len,in))!=-1)// cycle works until we dont read any char "getline()"-library function
    {
        if(!check_break(tmp_line,read))// checking if the line is not empty
        {
            tmp_line=remove_comments(tmp_line);  // this function removes comments from current line
            int count; //counter for cycle
            for(count=0;count<identif_counter;count++)  //this cycle goes through all identifiers that are known and looks if they are used as operands
            {
                if(check_used(tmp_line,identifiers[count])) //check if there a match with any identifier in this line
                {
                    char buf[10];   //temporary buffer for converting integer in string
                    sprintf(buf,"%d \0",linecount);// writes number of current line in "buf" variable
                    if(used[count]==NULL)   // if used is uninitialized
                        strcpy(used[count],buf); // put first number
                    else
                        strcat(used[count],buf); // otherwise append it to another numbers
                }
                
            }
            if(strstr(tmp_line,":")!=NULL) //if we have a ':' char it seems we have an identifier defined
            {
                int i=-1,j=0;   // counters for cycles
                while(isspace(tmp_line[++i]));  //going through whitespaces before text
                
                while(tmp_line[i+j]!=':')   //cycle gets all chars before ':'
                {
                    identifiers[identif_counter][j]= tmp_line[i+j]; //copying chars into identifier
                    j++;
                }
                
                identifiers[identif_counter][j]='\0';	//end of string
                defined[identif_counter]=linecount;	// adding current line number to defined value of this identifier
                identif_counter++;			// increment indentifier counter
            }
            
            linecount++; // increasing counter of line			
        }
    }
    int i; //just counter for cycle
    for(i=0;i<identif_counter;i++)// cycle goes through all identifiers and outputs values for them
    {
        fprintf(out,"%s\t\t%d\t\t%s\n",identifiers[i],defined[i],used[i]);
    }
}
