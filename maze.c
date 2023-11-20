#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

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
}Maze;

int step(Maze *maze, int *row, int *col, int leftright, int *dir);


bool strequal(char *str1,char str2[])
{
 int i=0;
 while(str1[i]==str2[i] && str1[i]!='\0' && str2[i]!='\0')i++;
 if(str1[i]!='\0' || str2[i]!='\0')return false;
 return true;
}


bool isborder(Maze *maze, int row, int col, int border)
{
 unsigned char mask;
 if(border==LEFTODD || border==LEFTEVEN)mask=1;else
  if(border==RIGHTODD || border==RIGHTEVEN)mask=1<<1;else mask=1<<2;
 if((maze->cells[(row-1)*maze->cols+(col-1)] & mask)!=0)return true;
 return false;
}

int iteration(Maze *maze, int *helpmaze, int *queue)
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

int shortest(Maze *maze,int row, int col)
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
 //manual
 return 0;
}


int testfunction(Maze *maze)
{
 for(int row=1;row<=maze->rows;row++)
 {
  for(int col=1;col<=maze->cols;col++)
  {
   if(col!=1 && isborder(maze,row,col,LEFTODD) && !isborder(maze,row,col-1,RIGHTODD))return false;
   if(col!=maze->cols && isborder(maze,row,col,RIGHTODD) && !isborder(maze,row,col+1,LEFTODD))return false;
   if((row+col)%2==0 && row!=1 && isborder(maze,row,col,TOP) && !isborder(maze,row-1,col,BOTTOM))return false;
   if((row+col)%2==1 && row!=maze->rows && isborder(maze,row,col,BOTTOM) && !isborder(maze,row+1,col,TOP))return false;
  }
 }
 return true;
}


int startborder(Maze *maze,int row,int col,int leftright)
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

int step(Maze *maze, int *row, int *col, int leftright, int *dir)
{
 int i=0,d,newdir;
 if(leftright==RIGHTHAND)d=1;else d=DIRECTIONS-1;// misto -1
 //printf("Here *dir is %d\n",*dir);
 while(i%DIRECTIONS != *dir)i+=d;  
  while(isborder(maze,*row,*col,i%DIRECTIONS))i+=2*(DIRECTIONS-d);
  *dir=(i%DIRECTIONS);
  //printf("And here direction is %d\n",*dir);
  newdir=(i+d)%DIRECTIONS;
  //printf("New direction is %d\n",newdir);
 
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

int entermaze(Maze *maze, int row, int col, int leftright)
{
 bool valid=testfunction(maze);
 if(!valid)return 1;
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
 if(strequal(argv[1],"--help"))return helpfunction();else
  if(strequal(argv[1],"--rpath"))lr=RIGHTHAND;else
   if(strequal(argv[1],"--lpath"))lr=LEFTHAND;else
    if(strequal(argv[1],"--shortest"))lr=SHORTEST;else	
	if(!strequal(argv[1],"--test"))//dopsat pro --shortest dole
	{
   	 fprintf(stderr,"Invalid first argument!\n");
   	 return 0;
  	}
 Maze maze={0,0,NULL};
 FILE *mazefile = fopen(argv[argc-1],"r");
 if(mazefile==NULL)
 {
  fprintf(stderr,"Chyba v souboru!\n");	 
  return close(mazefile,maze.cells);
 }
 int c;
 while((c=getc(mazefile))!='\n')
 {
  static int state=SKIPPING;//stav cteni
  if(!isdigit(c) && c!=' ')
  {
   fprintf(stderr,"V souboru bludiste znaky mimo 0123456789, ' ' a '\\n' jsou zakazane!\n");	  
   return close(mazefile,maze.cells);//chyba znaku
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
   return close(mazefile,maze.cells);//chyba znaku
  }
  if(i==maze.rows*maze.cols && k==1)
  {
   fprintf(stderr,"V danem bludisti je jen %d policek, mate vice!",maze.rows*maze.cols);  
   return close(mazefile,maze.cells);//chyba poctu
  }
  if(k==1)
  {
   maze.cells[i]=c;
   i++;
  }
 }
 if(strequal(argv[1],"--test"))
 {
  if(!testfunction(&maze))printf("Invalid\n");else printf("Valid\n");
    return close(mazefile,maze.cells);
 }
 if(argv[2]==NULL || argv[3]==NULL)
 {
  fprintf(stderr,"Zadejte radek a sloupec pri vstupu!\n");
  return close(mazefile,maze.cells);
 }
 int i=0;
 while(isdigit(argv[2][i]) && argv[2][i]!='\0')i++;
 if(argv[2][i]!='\0' || atoi(argv[2])==0)
 {
  fprintf(stderr,"Neplatny radek!\n");
  return close(mazefile,maze.cells);
 }
 i=0;
 while(isdigit(argv[3][i]) && argv[3][i]!='\0')i++;
 if(argv[3][i]!='\0' || atoi(argv[3])==0)
 {
  fprintf(stderr,"Neplatny sloupec!\n");
  return close(mazefile,maze.cells);
 }
 if(entermaze(&maze,atoi(argv[2]),atoi(argv[3]),lr)==1)fprintf(stderr,"Tady neni vstup!\n");//indexy jsou docasne
 free(maze.cells);
 fclose(mazefile);
 return close(mazefile,maze.cells);											    
}
