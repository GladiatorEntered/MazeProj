#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define DIRECTIONS 6
enum dir{LEFTEVEN, LEFTODD, TOP, RIGHTODD, RIGHTEVEN, BOTTOM, ERROR};
enum method{RIGHTHAND, LEFTHAND, SHORTEST};

int close(FILE *mazefile, unsigned char *cells)
{
 if(mazefile!=NULL)fclose(mazefile);
 if(cells!=NULL)free(cells);
 return 0;
}

typedef struct maze
{
 int rows;
 int cols;
 unsigned char *cells;
}Map;

bool isborder(Map *maze, int row, int col, int border)
{
 unsigned char mask;
 if(border==LEFTODD || border==LEFTEVEN)mask=1;else
  if(border==RIGHTODD || border==RIGHTEVEN)mask=1<<1;else mask=1<<2;
 if((maze->cells[(row-1)*maze->cols+(col-1)] & mask)!=0)return true;
 return false;
}

int iteration(Map *maze, int *helpmaze, int *queue)
{
 int *newqueue=malloc(maze->rows*maze->cols*sizeof(int));
 for(int i=0;i<maze->rows*maze->cols;i++)newqueue[i]=0;
 int j=0;
 static int steps=2; 
 for(int element=0;queue[element]!=0;element++)
 {
  int cellrow=((queue[element]-1)/maze->cols)+1;
  int cellcol=((queue[element]-1)%maze->cols)+1;
  if(!isborder(maze,cellrow,cellcol,RIGHTODD))//prevod do tvaru 0..n-1 a zpet na 1..n 
  {
   if(cellcol==maze->cols && helpmaze[queue[element]-1]!=1)
   {
    free(newqueue);	   
    return queue[element];
   }
   if(cellcol!=maze->cols && helpmaze[queue[element]]==0)//policko napravo==0?
   {
    helpmaze[queue[element]]=steps;
    newqueue[j++]=queue[element]+1;    
   }
  }
  if(!isborder(maze,cellrow,cellcol,LEFTODD))
  {
   if(cellcol==1 && helpmaze[queue[element]-1]!=1)
   {
    free(newqueue);
    return queue[element];
   }
   if(cellcol!=1 && helpmaze[queue[element]-2]==0)//policko nalevo==0?
   {
    helpmaze[queue[element]-2]=steps;
    newqueue[j++]=queue[element]-1;
   }
  }
  if(!isborder(maze,cellrow,cellcol, BOTTOM))//BOTTOM a UP maji stejny output ve funkci isborder()
  {
   if((cellrow+cellcol)%2)//je mozno necim nahradit
   {
    if(cellrow==maze->rows && helpmaze[queue[element]-1]!=1)
    {
     free(newqueue);    
     return queue[element];
    }
    if(cellrow!=maze->rows && helpmaze[queue[element]-1+maze->cols]==0)
    {
     helpmaze[queue[element]-1+maze->cols]=steps;
     newqueue[j++]=queue[element]+maze->cols;
    }
   }else
   {
    if(cellrow==1 && helpmaze[queue[element]-1]!=1)
    {
     free(newqueue);
     return queue[element];
    }
    if(cellrow!=1 && helpmaze[queue[element]-1-maze->cols]==0)
    {
     helpmaze[queue[element]-1-maze->cols]=steps;//levou cat je mozno vymenin za konstatnu
     newqueue[j++]=queue[element]-maze->cols;
    }
   }
  }
 }
 steps++;
 if(j==0)return -1;//neni-li nalezen vystup
 for(int i=0;newqueue[i]!=0;i++)queue[i]=newqueue[i];
 free(newqueue);	
 return 0;
}

int shortest(Map *maze,int row, int col)
{
 int *helpmaze=malloc(maze->rows*maze->cols*sizeof(int));
 int *path=malloc(2*maze->rows*maze->cols*sizeof(int));
 for(int i=0;i<maze->rows*maze->cols;i++)helpmaze[i]=0;
 helpmaze[(row-1)*maze->cols+col-1]=1;
 int *queue=malloc(maze->rows*maze->cols*sizeof(int));
 for(int i=0;i<maze->rows*maze->cols;i++)queue[i]=0;				 
 queue[0]=(row-1)*maze->cols+col;//cislo policka 1..n 
 int j=0,cell;
 while((cell=iteration(maze,helpmaze,queue))==0)j++;
 j=0;
 if(cell==-1)cell=(row-1)*maze->cols+col;
 int cellrow=((cell-1)/maze->cols)+1,cellcol=((cell-1)%maze->cols)+1;
 while(helpmaze[cell-1]!=0)
 {
  cell=(cellrow-1)*maze->cols+cellcol;
  path[j++]=cellcol;
  path[j++]=cellrow;  
  if(helpmaze[cell-1]==1)break;
  int parita=((cellrow+cellcol)%2==0)?1:(-1); 
  if(cellcol!=maze->cols && !isborder(maze,cellrow,cellcol,RIGHTODD))//nahradit dvojity if &&
  {
   if(helpmaze[cell]==helpmaze[cell-1]-1)
   {
    cellcol++;	   
    continue;
   }
  }
  if(cellcol!=1 && !isborder(maze,cellrow,cellcol,LEFTODD))
  {
   if(helpmaze[cell-2]==helpmaze[cell-1]-1)
   {
    cellcol--;   
    continue;
   }
  }
  if(cellrow!=((parita+1)/2+maze->rows-1)%maze->rows+1 && !isborder(maze,cellrow,cellcol,BOTTOM)) //parita=1 => ((parita+1)/2+maze->rows-1+)%maze->rows+1=1; parita=-1 => -||- = maze->rows
  {
   if(helpmaze[cell-1-parita*maze->cols]==helpmaze[cell-1]-1)
   {
    cellrow-=parita;
    continue;
   }
  }
 }
 for(int element=j-1;(element-1)>=0;element-=2)printf("%d,%d\n",path[element],path[element-1]);
 free(path);
 free(queue);
 free(helpmaze);	
 return 0;
}

int helpfunction()
{
 printf("---HI THERE! THIS PROGRAM IS THE SECOND GladiatorEntered'S PROGRAMMING PROJECT, BELOW YOU CAN READ HOW TO USE IT PROPERLY---\n\n\n");
 printf("-MOTIVATION-\nPerhaps you have a maze (M rows x N columns) with triangular cells defined with 3-bit numbers. The rightmost bit defines if the cell has a wall on its LEFT border, the one in the middle describes RIGHT border, the leftmost bit describes UPPER/LOWER border. We assume, that the cell with coordinates 1,1 has UPPER border\nThe program solves your maze, it can find the way out using RIGHT HAND RULE or LEFT HAND RULE. It laso can find the SHORTEST path through the maze or test if your maze is valid.\n\n"); 
 printf("-HOW TO USE-\nPlace text file with your maze in the same folder with this program. Text in the file should look like this:\n");
 printf("M N\nX X . . . X\nX X . . . X\n. .       .\n. .       .\n. .       .\nX X . . . X\n\n");
 printf("Then start the program in a bash command line with one of those sets of arguments:\n");
 printf("\t--help                           To see the tutorial\n");
 printf("\t--test your_maze.txt             To test if your file describes a valid maze\n");
 printf("\t--rpath R C your_maze.txt        To show the way out from the cell at R-th row and C-th column, using RIGHT HAND RULE\n");
 printf("\t--lpath R C your_maze.txt        To show the way out from the cell at R-th row and C-th column, using LEFT HAND RULE\n");
 printf("\t--shortest R C your_maze.txt     To show the SHORTEST path through the maze\n\n");
 printf("-\"WHY IS MY FILE INVALID???\"-\n");
 printf("Specially for these cases I have some tips for you:\n");
 printf("\t1. Check if the file doesn't contain other characters exept 0,1,2,3,4,5,6,7,8,9,' ','\\n'\n");
 printf("\t2. Make sure, that all the numbers describing cells are 3-bit (in interval of 0-7)\n");
 printf("\t3. Your maze has at least one entrance, doesn't it?\n\n");
 printf("-\"PROGRAM FAILS WITH SOME ERROR!!!\"-\n");
 printf("Errors that can orrur:\n");
 printf("\tInvalid first argument - make sure you wrote it correctly\n");
 printf("\tFile error - failed to open your file. Check if its name is correct and if it is pleced in the same directory with the program\n");
 printf("\tInvalid row/column - there is no such a row or a column with this number\n");
 printf("\tNo entrance coordinates given - next time type coordinates before starting the program\n");
 printf("\tThere is no entranse here - the cell at R-th row and C-th column is not an entrance\n");
 printf("\tInvalid - your file is invalid\n\n");
 printf("------------------------------------------------------------------\n\n");
 //manual
 return 0;
}


int testfunction(Map *maze)
{
 bool hasentrance=false; 
 for(int row=1;row<=maze->rows;row++)
 {
  for(int col=1;col<=maze->cols;col++)
  {   
   int cell=(row-1)*maze->cols+(col-1);	
   if((cell==0 || cell==maze->cols-1 || cell==maze->cols*(maze->rows-1) || cell==maze->rows*maze->cols-1) && maze->cells[cell]==0)maze->cells[cell]=1<<2;
   if((col==1 && !isborder(maze,row,col,LEFTODD)) || (col==maze->cols && !isborder(maze,row,col,LEFTODD)) || (row==1 && (row+col)%2==0 && !isborder(maze,row,col,TOP)) || (row==maze->rows && (row+col)%2==1 && !isborder(maze,row,col,BOTTOM)))hasentrance=true; 
   if(maze->cells[(row-1)*maze->cols+col-1]==(unsigned char)~0)return false;
   if(col!=1 && isborder(maze,row,col,LEFTODD) && !isborder(maze,row,col-1,RIGHTODD))return false;
   if(col!=maze->cols && isborder(maze,row,col,RIGHTODD) && !isborder(maze,row,col+1,LEFTODD))return false;
   if((row+col)%2==0 && row!=1 && isborder(maze,row,col,TOP) && !isborder(maze,row-1,col,BOTTOM))return false;
   if((row+col)%2==1 && row!=maze->rows && isborder(maze,row,col,BOTTOM) && !isborder(maze,row+1,col,TOP))return false;
  }
 }
 return hasentrance;
}


int startborder(Map *maze,int row,int col,int leftright)
{
 bool lr = (leftright==RIGHTHAND);
 if(col==1 && !isborder(maze,row,col,LEFTODD))
 {
  if(row%2)return (lr)? RIGHTEVEN:TOP;else return (lr)? BOTTOM:RIGHTODD;
 }else if(col==maze->cols && !isborder(maze,row,col,RIGHTODD))
 {
  if(col%2)
  {
   if(row%2)return (lr)? TOP:LEFTEVEN;else return (lr)? LEFTODD:BOTTOM;
  }else if(row%2)return (lr)? LEFTODD:BOTTOM;else return (lr)? TOP:LEFTEVEN;
 }else if(row==1 && col%2 && !isborder(maze,row,col,'u'))return (lr)? LEFTEVEN:RIGHTEVEN;else
   if (row==maze->rows && col%2==(1+maze->rows%2)%2 && !isborder(maze,row,col,BOTTOM))return (lr)? RIGHTODD:LEFTODD;
 return ERROR;
}

int step(Map *maze, int *row, int *col, int leftright, int *dir)
{
 int i=*dir,d,newdir;
 if(leftright==RIGHTHAND)d=1;else d=DIRECTIONS-1;// misto -1
 while(isborder(maze,*row,*col,i%DIRECTIONS))i+=2*(DIRECTIONS-d);
 *dir=(i%DIRECTIONS);
 newdir=(i+d)%DIRECTIONS;
  
 switch(*dir)
 {
  case LEFTODD:  
  case LEFTEVEN:
	  (*col)--;
	  break;
  case RIGHTODD:
  case RIGHTEVEN:
	  (*col)++;
	  break;
  case BOTTOM:
	  (*row)++;
	  break;
  case TOP:
	  (*row)--;
	  break;
 }
 if(*row==0 || *row>maze->rows || *col==0 || *col>maze->cols)return 1;
 *dir=newdir;
 return 0;
}

int entermaze(Map *maze, int row, int col, int leftright)
{
 bool valid=testfunction(maze);
 if(!valid)return 2;
 if(leftright==SHORTEST)return shortest(maze,row,col);
 int dir = startborder(maze,row,col,leftright);
 if(dir==ERROR)return 1;//neni vstup
 printf("%d,%d\n", row, col);
 while(step(maze,&row,&col,leftright,&dir)==0)printf("%d,%d\n",row,col);
 return 0;
}

enum readingmode{SKIPPING, READ_ROWS, READ_COLS};

int main(int argc, char *argv[])
{
 int lr, k=0;	
 if(strcmp(argv[1],"--help")==0)return helpfunction();else
  if(strcmp(argv[1],"--rpath")==0)lr=RIGHTHAND;else
   if(strcmp(argv[1],"--lpath")==0)lr=LEFTHAND;else
    if(strcmp(argv[1],"--shortest")==0)lr=SHORTEST;else	
	if(strcmp(argv[1],"--test")!=0)//dopsat pro --shortest dole
	{
   	 fprintf(stderr,"Invalid first argument!\n");
   	 return 0;
  	}
 Map maze={0,0,NULL};
 FILE *mazefile = fopen(argv[argc-1],"r");
 if(mazefile==NULL)
 {
  fprintf(stderr,"File error!\n");	 
  return close(mazefile,maze.cells);
 }
 int c,dims=0;
 while(dims != 2)
 {
  c=fgetc(mazefile);
  static int state=SKIPPING;//stav cteni
  if(!isdigit(c) && c!=' ' && c!='\n')
  {
   maze.rows=0;
   maze.cols=0;
   break;   
  }
  if(state==SKIPPING && isdigit(c))
  {
   if(maze.rows==0)state=READ_ROWS;else state=READ_COLS;
  }
  if(state!=SKIPPING && !isdigit(c))
  {
   state=SKIPPING;
   dims++;
  }
  if(state==READ_ROWS)
  {
   maze.rows*=10;
   maze.rows+=(c-'0');
  }
  if(state==READ_COLS)
  {
   maze.cols*=10;
   maze.cols+=(c-'0');
  }
 }
 maze.cells=(unsigned char*)malloc((maze.rows*maze.cols+1)*sizeof(unsigned char));
 int i=0;
 maze.cells[0]=~0; 
 while((c=getc(mazefile))!=EOF)
 {
  if(c==' ' || c=='\n')k=0;else k++;
  if(k>1 || c>='8')
  {
   maze.cells[0]=~0;	  
   break;
  }
  if(i==maze.rows*maze.cols && k==1)
  {
   maze.cells[0]=~0;
   break;
  }
  if(k==1)
  {
   maze.cells[i]=(c-'0');
   i++;
  }
 }
 if(i!=maze.rows*maze.cols)maze.cells[0]=~0;
 if(strcmp(argv[1],"--test")==0)
 {
  if(!testfunction(&maze))printf("Invalid\n");else printf("Valid\n");
    return close(mazefile,maze.cells);
 }
 if(argv[2]==NULL || argv[3]==NULL)
 {
  fprintf(stderr,"No entrance coordinates given!\n");
  return close(mazefile,maze.cells);
 }
 i=0;//pouziva se jako counter
 while(isdigit(argv[2][i]) && argv[2][i]!='\0')i++;
 if(argv[2][i]!='\0' || atoi(argv[2])<1 || atoi(argv[2])>maze.rows)
 {
  fprintf(stderr,"Invalid row!\n");
  return close(mazefile,maze.cells);
 }
 i=0;
 while(isdigit(argv[3][i]) && argv[3][i]!='\0')i++;
 if(argv[3][i]!='\0' || atoi(argv[3])<1 || atoi(argv[3])>maze.cols)
 {
  fprintf(stderr,"Invalid column!\n");
  return close(mazefile,maze.cells);
 }
 if((atoi(argv[2])!=1 && atoi(argv[2])!=maze.rows && atoi(argv[3])!=1 && atoi(argv[3])!= maze.cols) || (i=entermaze(&maze,atoi(argv[2]),atoi(argv[3]),lr))!=0)
 {
  switch(i)
  {
   case 0:
   case 1:	   
     fprintf(stderr,"There is no entrance here!\n");//indexy jsou docasne
     break;
   case 2:
     fprintf(stderr,"Invalid\n");
  }
 }
  return close(mazefile, maze.cells);;											    
}
