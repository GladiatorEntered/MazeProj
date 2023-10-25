#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct maze
{
 int rows;
 int cols;
 unsigned char *cells;
}Maze;

int step(Maze *maze, int *row, int *col, int leftright, int *dir);

bool isborder(Maze *maze, int row, int col, int border)
{
 unsigned char c;
 if(border=='l')c=1;else
  if(border=='r')c=1<<1;else c=1<<2;
 if((maze->cells[(row-1)*maze->cols+(col-1)] & c)!=0)return true;
 return false;
}

int startborder(Maze *maze,int r,int c,int leftright)
{
 bool lr = (leftright=='r');	
 if(c==1 && !isborder(maze,r,c,'l'))
 {
  if(r%2)return (lr)? 'r':'u';else return (lr)? 'b':'r';
 }else if(c==maze->cols && !isborder(maze,r,c,'r'))
 {
  if(c%2)
  {
   if(r%2)return (lr)? 'u':'l';else return (lr)? 'l':'b';
  }else if(r%2)return (lr)? 'l':'b';else return (lr)? 'u':'l';
 }else if(r==1 && c%2 && !isborder(maze,r,c,'u'))return (lr)? 'l':'r';else
   if (r==maze->rows && c%2==(1+maze->rows%2)%2 && !isborder(maze,r,c,'b'))return (lr)? 'r':'l';
 return 0;
}

void entermaze(Maze *maze, int row, int col, int leftright)
{
 int dir = startborder(maze,row,col,leftright);
 printf("%d %d\n", row, col);
 while(step(maze,&row,&col,leftright,&dir)!=0)printf("%d %d\n",row,col);
}

int step(Maze *maze, int *row, int *col, int leftright, int *dir)
{
 int odd[]={'l','b','r'};
 int even[]={'l','r','u'};
 int i=0, d,newdir;
 if(leftright=='r')d=1;else d=2;//2 je misto -1
 fprintf(stderr,"Here d=%d\n",d);//log1
 if((*row + *col)%2)
 {
  fprintf(stderr,"The cell is odd here\n");//log2	 
  while(*dir != odd[i%3])
  {
   i=i+d;
   fprintf(stderr,"Here i=%d\n",i);//log3
   if(i>20)exit(0);				   
  }
  while(isborder(maze,*row,*col,odd[i%3]))
  {
   i=i+d;
   fprintf(stderr,"Hete i=%d\n",i);//log4
   if(i>20)exit(0);
  }
  *dir=odd[i%3];
  fprintf(stderr,"Now *dir=%c\n",*dir);//log5
  newdir=(d==1)?even[(i%3+2)%3]:even[i%3];//+2 je misto -1
  fprintf(stderr,"Now newdir=%c\n",newdir);//log6
 }else
 {
  fprintf(stderr,"The cell is even here\n");	 
  while(*dir != even[i%3])
  {
   i=i+d;
   fprintf(stderr,"Here i=%d\n",i);
   if(i>20)exit(0);
  }
  while(isborder(maze,*row,*col,even[i%3]))
  {
   i=i+d;
   fprintf(stderr,"Hete i=%d\n",i);
   if(i>20)exit(0);
  }
  *dir=even[i%3];
  fprintf(stderr,"Now *dir=%c\n",*dir);
  newdir=(d==1)?odd[i%3]:odd[(i%3+1)%3];
  fprintf(stderr,"Now newdir=%c\n",newdir);
 }
 fprintf(stderr,"Before incrementation *col=%d, *dir=%c\n",*col,*dir);//log7
 if(*dir=='l')(*col)--;else
  if(*dir=='r')(*col)++;else
   if(*dir=='u')(*row)--;else
    if(*dir=='b')(*row)++;
 fprintf(stderr,"After incrementation *col=%d\n",*col);//log9
 fprintf(stderr,"*row=%d *col=%d *dir=%c\n",*row,*col,*dir);//log8
 if(*row==0 || *row>maze->rows || *col==0 || *col>maze->cols)return 0;
 *dir=newdir;
  return 1;
}

enum readingmode{SKIPPING, READ_ROWS, READ_COLS};

int main(int argc, char *argv[])
{
 Maze maze={0,0,NULL};
 int k=0;
 FILE *mazefile = fopen(argv[argc-1],"r");
 if(mazefile==NULL)return 0;//chyba
 int c;
 while((c=getc(mazefile))!='\n')
 {
  static int state=SKIPPING;//stav cteni
  if(!isdigit(c) && c!=' ')return 0;//chyba znaku
  if(state==SKIPPING && isdigit(c))
  {
   if(maze.rows==0)state=READ_ROWS;else state=READ_COLS;
  }
  if(state!=SKIPPING && !isdigit(c))state=SKIPPING;
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
 maze.cells=(unsigned char*)malloc(maze.rows*maze.cols*sizeof(unsigned char));
 fprintf(stderr,"Capacity=%d\n",maze.rows*maze.cols);
 while((c=getc(mazefile))!=EOF)
 {
  static int i=0;
  if(c==' ' || c=='\n')k=0;else k++;
  if(k>1 || c>='8')return 0;//chyba znaku
  if(i==maze.rows*maze.cols && k==1)return 0;//chyba poctu			    
  if(k==1)
  {
   fprintf(stderr,"Now i=%d, cells[%d]=%c\n",i,i,c);	  
   maze.cells[i]=c;
   i++;
  }
 }
 int lr=argv[1][0];//kostyl
entermaze(&maze,atoi(argv[2]),atoi(argv[3]),lr);//indexy jsou docasne
}
