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

bool strequal(char *str1,char str2[])
{
 int i=0;
 while(str1[i]==str2[i] && str1[i]!='\0' && str2[i]!='\0')i++;
 if(str1[i]!='\0' || str2[i]!='\0')return false;
 return true;
}

int helpfunction()
{
 //manual
 return 0;
}

int testfunction()
{
 //test
 return 0;
}

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

int entermaze(Maze *maze, int row, int col, int leftright)
{
 int dir = startborder(maze,row,col,leftright);
 if(dir==0)return 1;//neni vstup
 printf("%d %d\n", row, col);
 while(step(maze,&row,&col,leftright,&dir)==0)printf("%d %d\n",row,col);
 return 0;
}

int step(Maze *maze, int *row, int *col, int leftright, int *dir)
{
 int odd[]={'l','b','r'};
 int even[]={'l','r','u'};
 int i=0, d,newdir;
 if(leftright=='r')d=1;else d=2;//2 je misto -1
 if((*row + *col)%2)
 {
  while(*dir != odd[i%3])i=i+d;  
  while(isborder(maze,*row,*col,odd[i%3]))i=i+d;
  *dir=odd[i%3];
  newdir=(d==1)?even[(i%3+2)%3]:even[i%3];//+2 je misto -1
  }else
 {
  while(*dir != even[i%3])i=i+d;  
  while(isborder(maze,*row,*col,even[i%3]))i=i+d;//hledame kam jit, tocime se
  *dir=even[i%3];//kam jdeme
  newdir=(d==1)?odd[i%3]:odd[(i%3+1)%3];//dale hledame hranici tamto
 }
 if(*dir=='l')(*col)--;else //krok
  if(*dir=='r')(*col)++;else
   if(*dir=='u')(*row)--;else
    if(*dir=='b')(*row)++;
 if(*row==0 || *row>maze->rows || *col==0 || *col>maze->cols)return 1;
 *dir=newdir;
 return 0;
}

enum readingmode{SKIPPING, READ_ROWS, READ_COLS};

int main(int argc, char *argv[])
{
 int lr, k=0;	
 if(strequal(argv[1],"--help"))return helpfunction();else
  if(strequal(argv[1],"--rpath"))lr='r';else
   if(strequal(argv[1],"--lpath"))lr='l';else if(!strequal(argv[1],"--test"))
   {
    fprintf(stderr,"Invalid first argument!\n");
    return 0;
   }
 Maze maze={0,0,NULL};
 FILE *mazefile = fopen(argv[argc-1],"r");
 if(mazefile==NULL)
 {
  fprintf(stderr,"Chyba v souboru!\n");	 
  return 0;
 }
 int c;
 while((c=getc(mazefile))!='\n')
 {
  static int state=SKIPPING;//stav cteni
  if(!isdigit(c) && c!=' ')
  {
   fprintf(stderr,"V souboru bludiste znaky mimo 0123456789, ' ' a '\\n' jsou zakazane!\n");	  
   return 0;//chyba znaku
  }
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
 while((c=getc(mazefile))!=EOF)
 {
  static int i=0;
  if(c==' ' || c=='\n')k=0;else k++;
  if(k>1 || c>='8')
  {
   fprintf(stderr,"Bludiste muze byt popsano jen 3bitovymy cisly!\n");	  
   return 0;//chyba znaku
  }
  if(i==maze.rows*maze.cols && k==1)
  {
   fprintf(stderr,"V danem bludisti je jen %d policek, mate vice!",maze.rows*maze.cols);  
   return 0;//chyba poctu
  }
  if(k==1)
  {
   maze.cells[i]=c;
   i++;
  }
 }
 if(strequal(argv[1],"--test"))return testfunction();
 if(argv[2]==NULL || argv[3]==NULL)
 {
  fprintf(stderr,"Zadejte radek a sloupec pri vstupu!\n");
  return 0;
 }
 int i=0;
 while(isdigit(argv[2][i]) && argv[2][i]!='\0')i++;
 if(argv[2][i]!='\0' || atoi(argv[2])==0)
 {
  fprintf(stderr,"Neplatny radek!\n");
  return 0;
 }
 i=0;
 while(isdigit(argv[3][i]) && argv[3][i]!='\0')i++;
 if(argv[3][i]!='\0' || atoi(argv[3])==0)
 {
  fprintf(stderr,"Neplatny sloupec!\n");
  return 0;
 }
 if(entermaze(&maze,atoi(argv[2]),atoi(argv[3]),lr)==1)fprintf(stderr,"Tady neni vstup!\n");//indexy jsou docasne
 return 0;											    
}
