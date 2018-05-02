#include <cairo.h>
#include <gtk/gtk.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>


#define width   600
#define height  600
#define MAXCHAR 4194306
#define w_dimention .2
#define f_dimention .15
static void do_drawing(cairo_t *);

int points[11][2] = {
    { 0, 85 },
    { 75, 75 },
    { 100, 10 },
    { 125, 75 },
    { 200, 85 },
    { 150, 125 },
    { 160, 190 },
    { 100, 150 },
    { 40, 190 },
    { 50, 125 },
    { 0, 85 }
};

int **grid;
 int rows = 1;
 int cols = 1;

 int count=0;

 float scaleFactor = 10;
 float d_angle=0;
 float angle_top=1.57*4;

GtkFileChooserButton   *bt_fileCh;

GtkButton* go_bt;
GtkEntry*  entryFila;
GtkEntry*  entryColumna;
GtkLabel*  lb_prompt;
GtkLabel*  lb_n;
GtkLabel*  lb_m;
GtkLabel*  lb_resolver;
GtkWidget *sub_window;

char* bin_nm;
char st1[4]="";
char st2[4]="";
int count_current_file;

FILE *desc;

int is_grabar=0;

 gdouble scroll_x;
 gdouble scroll_y;
 gdouble mouse_x;
 gdouble mouse_y;
//--------------------------------------------------------------------------


int col;
int fil;

int ** matriz;
int *  filas;
int * columnas;
int * estados;

int opciones[4];
int *spanning;
int begin[2];
int spanningSize =0;

//=----------------------------------------------------------------------
struct GdkEventScroll {
  GdkEventType type;
  GdkWindow *window;
  gint8 send_event;
  guint32 time;
  gdouble x;
  gdouble y;
  guint state;
  GdkScrollDirection direction;
  GdkDevice *device;
  gdouble x_root, y_root;
  gdouble delta_x;
  gdouble delta_y;
  guint is_stop : 1;
};
//=----------------------------------------------------------------------

/*
*1 barrera derecha borrada
		*2 barrera de abajo
		*4 izquierda
		*8 arriba
*/

//_________________________________MANEJO DE LOS DATOS_____________________________________________________
void initMaze()
{
    for(int i=0; i<fil; i++)
    {
        for(int j=0; j<col; j++)
        {
            matriz[i][j] = 0;
        }
    }
}

void initReferences()
{
    for(int i=0; i<fil; i++)
    {
        for(int j=0; j<col; j++)
        {
            filas[ (i* (col-1) )+(i+j) ]=i;
            columnas[ (i*(col-1))+(i+j) ] = j;
        }

    }
}

void initEstados()
{
    for(int k=0; k<fil*col;k++)
    {
        estados[k] = 0;
    }
}

void tagBorders(int fila, int columna)
{
    int index;
    if( fila-1 > -1 )
    {
        index = ( (fila-1)*(col-1)+(fila-1)+(columna) );
        if( !estados[index] )
        {
            estados[index] = 2;
        }
    }
    if( fila+1 < fil )
    {
        index = ( (fila+1)*(col-1)+(fila+1)+(columna) );
        if( !estados[index] )
        {
            estados[index] = 2;
        }
    }
    if( columna-1 > -1 )
    {
        index = ( (fila)*(col-1)+(fila)+(columna-1) );
        if( !estados[index] )
        {
            estados[index] = 2;
        }
    }
    if( columna+1 < col )
    {
        index = ( (fila)*(col-1)+(fila)+(columna+1) );
        if( !estados[index] )
        {
            estados[index] = 2;
        }
    }
}

int adyacentValidCount(int fila, int columna)
{
    int validCount = 0;
    int index;
    if( fila-1 > -1 )
    {
        index = ( (fila-1)*(col-1)+(fila-1)+(columna) );
        if( estados[index]==2 )
        {
            validCount++;
        }
    }
    if( fila+1 < fil )
    {
        index = ( (fila+1)*(col-1)+(fila+1)+(columna) );
        if( estados[index]==2)
        {
            validCount++;
        }
    }
    if( columna-1 > -1 )
    {
        index = ( (fila)*(col-1)+(fila)+(columna-1) );
        if( estados[index]==2)
        {
            validCount++;
        }
    }
    if( columna+1 < col )
    {
        index = ( (fila)*(col-1)+(fila)+(columna+1) );
        if( estados[index]==2 )
        {
            validCount++;
        }
    }

    return validCount;
}


int calcElections(int fila, int columna)
{
    int opcionesSize = 0;
    if( adyacentValidCount(fila,columna) == 0 ) return -1;
    int index;
    if( fila-1 > -1 )
    {
        index = ( (fila-1)*(col-1)+(fila-1)+(columna) );
        if( estados[index]==2 )
        {
            opciones[opcionesSize] = 0;
            opcionesSize++;
        }
    }
    if( fila+1 < fil )
    {
        index = ( (fila+1)*(col-1)+(fila+1)+(columna) );
        if( estados[index]==2)
        {
            opciones[opcionesSize] = 1;
            opcionesSize++;
        }
    }

    if( columna-1 > -1 )
    {
        index = ( (fila)*(col-1)+(fila)+(columna-1) );
        if( estados[index]==2)
        {
            opciones[opcionesSize] = 2;
            opcionesSize++;
        }
    }
    if( columna+1 < col )
    {
        index = ( (fila)*(col-1)+(fila)+(columna+1) );
        if( estados[index]==2 )
        {
            opciones[opcionesSize] = 3;
            opcionesSize++;
        }
    }

    return opcionesSize;

}
void beginConection(int fila, int columna)
{
    //printf("\n\tEmpezamos");
    if( adyacentValidCount(fila,columna)==0 )
    {
        //printf("\n\tNo hay Conecciones");
        spanningSize--;
        return;
    }
    int option = opciones[ rand() % ( calcElections(fila, columna) )  ];
    //printf("\n\t La opcion es: %d", option);

    /*
    fila-1
    fila+1
    columna-1
    columna+1
    */
    /*
*1 barrera derecha borrada
		*2 barrera de abajo
		*4 izquierda
		*8 arriba
*/
    int index;
    if(!option)
    {
        matriz[fila][columna] += 8;
        matriz[fila-1][columna] +=2;

        index =( (fila-1)*(col-1)) + (fila-1+columna);
        estados[ index ] = 1;
        spanningSize++;
        spanning[spanningSize] = index;
        //printf("\n\tFila-1");
    }
    if(option == 1)
    {
        matriz[fila][columna] += 2;
        matriz[fila+1][columna] +=8;

        index =( (fila+1)*(col-1)) + (fila+1+columna);
        estados[ index ] = 1;
        spanningSize++;
        spanning[spanningSize] = index;
        //printf("\n\tFila+1");
    }
    if(option == 2)
    {
        matriz[fila][columna] += 4;
        matriz[fila][columna-1] +=1;

        index =( (fila)*(col-1)) + (fila+columna-1);
        estados[ index ] = 1;
        spanningSize++;
        spanning[spanningSize] = index;
        //printf("\n\tColumna-1");
    }
    if(option == 3)
    {
        matriz[fila][columna] += 1;
        matriz[fila][columna+1] += 4;

        index =( (fila)*(col-1)) + (fila+columna+1);
        estados[ index ] = 1;
        spanningSize++;
        spanning[spanningSize] = index;
       // printf("\n\tColumna+1");
    }
    //printf("\n\tFinal");
}

int allOne()
{
    for(int k= 0; k<fil*col;k++)
    {
        if(estados[k] != 1) return 0;
    }
    return 1;
}
//____________________________________________VERIFICACIÓN-----> LOS PRINTS_________________________________________________


void printMaze(){

    for(int i=0; i<fil; i++)
    {
        for(int j=0; j<col; j++) printf("\nM[%d][%d] = %d",i, j, matriz[i][j] );
    }
}
void printMaze2(){
  for(int i=0; i<fil; i++){
    printf("\n\t");
    for(int j=0; j<col; j++) printf("%d ", matriz[i][j]);
  }printf("\n");

}
void printReferences(){
    for(int i=0; i<fil; i++)
    {
        for(int j=0; j<col; j++)
        {
            int index = (i*(col-1))+(i+j);
        }
    }
}

void printEstados(){

    for(int i=0; i<fil; i++)
    {
        for(int j=0; j<col; j++)
        {
            int index = (i*(col-1))+(i+j);
        }
    }
}
void start_final_conection()
{
    int opcion = rand() % 1;

    if(!opcion)
    {
        matriz[0][0] += 4;
        matriz[fil-1][col-1] += 1;
    }
    if(opcion)
    {
        matriz[0][col-1] += 1;
        matriz[fil-1][0] += 4;
    }
}

//____________________________________________________EL_MAIN()___________________________________________________________-


void new_maze(){
  fil=rows;
  col=cols;

  matriz = (int**)malloc(fil * sizeof(int*));
  for(int e=0; e<fil; e++)matriz[e]=(int* )malloc(col * sizeof(int* ));

  filas = (int*)malloc( fil*col*sizeof(int*) );
  columnas = (int*)malloc( fil*col*sizeof(int*) );
  estados = (int*)malloc( fil*col*sizeof(int*) );

  initEstados();
  initMaze();
  initReferences();

  spanning = (int*)malloc( col*fil*sizeof(int*) );

  //______________________________________MANEJO_DE_DATOS_____________________________________________________


  begin[0] = rand() % fil;
  begin[1] = rand() % col;

  int index =(begin[0]*(col-1)) + (begin[0]+begin[1]);
  estados[ index ] = 1;
  spanning[spanningSize] = index;
  int actualFil = filas[ spanning[spanningSize] ];
  int actualCol = columnas[ spanning[spanningSize] ];
  tagBorders(actualFil, actualCol);

  int i=0;

  while( !allOne() ){
      tagBorders(actualFil, actualCol);
      beginConection( actualFil , actualCol );
      actualFil = filas[ spanning[spanningSize] ];
      actualCol = columnas[ spanning[spanningSize] ];
      i++;
  }
  start_final_conection();

  //printMaze2();

}




//=----------------------------------------------------------------------


struct GdkEventScroll scroll_handler;

int randomG(int bottom, int top){return (rand()%top + bottom);}

int** new_matrix(int m, int n){
  int **matrix = (int**)malloc(m* sizeof(int*));
  for (size_t i = 0; i < m; i++) {
    matrix[i] = (int*)malloc(n*sizeof(int));
  }return matrix;
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data){

  do_drawing(cr);

  return FALSE;
}

 int proportion(){
  int big;
  if(rows>=cols){big=rows;}else{big=cols;}
  return big;
}
char *decimal_to_binary(int n){

   int c, d, count;
   char *pointer;

   count = 0;
   pointer = (char*)malloc(4+1);

   if (pointer == NULL)
      exit(EXIT_FAILURE);

   for (c = 3 ; c >= 0 ; c--)
   {
      d = n >> c;

      if (d & 1)
         *(pointer+count) = 1 + '0';
      else
         *(pointer+count) = 0 + '0';

      count++;
   }
   *(pointer+count) = '\0';

   return  pointer;
}

void draw_walls(cairo_t *cr, float xc, float yc, int ic, int jc){
  float x = xc; float y=yc; float w=w_dimention;
  char* binary;
  int local_wall[4];


      binary = decimal_to_binary(matriz[ic][jc]);
      for (size_t e = 0; e < 4; e++) local_wall[e] =  binary[e]-'0';


      if(!local_wall[0]){//up
        cairo_set_source_rgb(cr, 0, .2, 0);
        cairo_move_to(cr, x, y);
        cairo_line_to(cr, x+w, y);
        cairo_stroke_preserve(cr);  cairo_fill(cr);

      }
      if(!local_wall[3]){//right
        cairo_set_source_rgb(cr, 0, .4, 0);
        cairo_move_to(cr, x+w, y);
        cairo_line_to(cr, x+w, y+w);
        cairo_stroke_preserve(cr);  cairo_fill(cr);

      }
      if(!local_wall[2]){//bottom
        cairo_set_source_rgb(cr, 0, .6, 0);
        cairo_move_to(cr, x+w, y+w);
        cairo_line_to(cr, x, y+w);
        cairo_stroke_preserve(cr);  cairo_fill(cr);
      }
      if(!local_wall[1]){//left
        cairo_set_source_rgb(cr, 0, .7, 0);
        cairo_move_to(cr, x, y+w);
        cairo_line_to(cr, x, y);
        cairo_stroke_preserve(cr);  cairo_fill(cr);
      }
      free(binary);
}


void draw_ES(cairo_t *cr, float w){

  //_____________START____________
  cairo_set_source_rgb(cr, 1, 0, 0);
  cairo_move_to(cr, 0, w);
  cairo_line_to(cr, 0, 0);
  cairo_stroke_preserve(cr);  cairo_fill(cr);
    //_____________END___________
  cairo_set_source_rgb(cr, 0, 0, 1);
  cairo_move_to(cr, w*(cols-1)+w, w*(rows-1));
  cairo_line_to(cr, w*(cols-1)+w, w*(rows-1)+w);
  cairo_stroke_preserve(cr);  cairo_fill(cr);
}



void grabar(){

  if(is_grabar){
    file_generator(rows, cols);
  }
  is_grabar = 0;
}

void draw_setos(cairo_t *cr, float x){
  for (size_t i = 0; i < 2; i++) {
    for (size_t j = 0; j < 2; j++) {
        cairo_set_source_rgb(cr, 0, (1+i+j)*.2, 0);
        cairo_rectangle(cr, (x+i)*.1, j*.1, .1, .1);
        cairo_stroke_preserve(cr);
        cairo_fill(cr);
    }
  }

}
void grid_test(cairo_t *cr){

  int local_filter=0;
  //float w = proportion();
  float w=w_dimention;
  //-------------------
  cairo_scale(cr, scaleFactor, scaleFactor);
  draw_ES(cr, w);

  for(int i=0; i<rows; i++){
    for (int j =0; j<cols; j++) {
      draw_walls(cr, j*w, i*w, i, j);
    }
  }
  cairo_stroke(cr);

}
static void do_drawing(cairo_t *cr){
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_set_line_width(cr, .03125);
  cairo_translate(cr, mouse_x, mouse_y);
  cairo_rotate (cr, d_angle);
  cairo_scale(cr, scaleFactor, scaleFactor);


  grid_test(cr);

}


void on_window_main_destroy(){gtk_main_quit();}

int get_int_len (int value){
  int l=1;
  while(value>9){ l++; value/=10; }
  return l;
}
char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int listdir(char *dir) {
  int file_count = 0;
  DIR * dirp;
  struct dirent * entry;

  dirp = opendir(dir); /* There should be error handling after this */
  while ((entry = readdir(dirp)) != NULL) {
      if (entry->d_type == DT_REG) { /* If the entry is a regular file */
           file_count++;
      }
  }
  closedir(dirp);
  return file_count;
}

void escritura(int r, int c, char* w_dir){
  char* f_name="";
  static char  calc[4]="maze";
  char  ndata[4];
  char* final="";
  char* f_pivot="";
  sprintf(ndata, "%d", count_current_file++);
  f_name = concat(calc, ndata);
  f_pivot = concat(w_dir, f_name);
  final = concat(f_pivot, ".txt");

  desc = fopen(final, "w");
  //===================
  if (desc == NULL) {
    /* code */
    printf("%s\n", "Error, no se puede crear el archivo");
  }else{
    fprintf(desc, "%d", r); fprintf(desc, "%s", "_");
    fprintf(desc, "%d", c); fprintf(desc, "%s", "_");

    for(int i=0; i<r; i++){
      for(int j=0; j<c; j++){
        fprintf(desc, "%d", matriz[i][j]);fprintf(desc, "%s", " ");
      }
    }
    fclose(desc);
  }rows = r; cols = c;
}

void file_generator(int n, int m){
    char  dir_wk[12] = "maze_data/";

   count_current_file = listdir(dir_wk);

   escritura(n, m, dir_wk);


 }
void check(){
  char* num;
  int result[2];
  int len_num;
  int len_result;
  int ready=1;



  num  =  gtk_entry_get_text (entryFila);
  len_num   = strlen(num);
  result[0] = atoi(num);
  len_result = get_int_len(result[0]);
  if(len_result != len_num || result[0]==0)ready=0;

  num  =  gtk_entry_get_text (entryColumna);
  len_num   = strlen(num);
  result[1] = atoi(num);
  len_result = get_int_len(result[1]);
  if(len_result != len_num || result[1]==0)ready=0;


  if(ready == 0 ){
    gtk_entry_set_text (GTK_ENTRY(entryFila), "");
    gtk_entry_set_text (GTK_ENTRY(entryColumna), "");
    gtk_label_set_text(GTK_LABEL(lb_prompt), "Entrada invalida ):");
  }
  if(result[0]>2048 || result[1]>2048){
    gtk_entry_set_text (GTK_ENTRY(entryFila), "");
    gtk_entry_set_text (GTK_ENTRY(entryColumna), "");
    gtk_label_set_text(GTK_LABEL(lb_prompt), "El tope es 2048x2048! ):");
    ready = 0;
  }

  if(ready){
    is_grabar = 1;

    float calc = (width/proportion())/proportion();
    scaleFactor= 24;
    gtk_window_close (sub_window);
    sprintf(st1, "%d", result[0]);
    sprintf(st2, "%d", result[1]);
    gtk_label_set_text(GTK_LABEL(lb_m), st1);
    gtk_label_set_text(GTK_LABEL(lb_n), st2);
    rows = result[0]; cols = result[1];
    new_maze();
  }

}

void generar(){

  GtkBuilder*  sub_builder;
  sub_builder = gtk_builder_new();
  gtk_builder_add_from_file (sub_builder, "glade/window_generar.glade", NULL);

  sub_window  = GTK_WIDGET(gtk_builder_get_object(sub_builder, "window_generar"));
  lb_prompt   = GTK_WIDGET(gtk_builder_get_object(sub_builder, "lb_prompt"));
  entryFila   = GTK_WIDGET(gtk_builder_get_object(sub_builder, "entry_fila"));
  entryColumna   = GTK_WIDGET(gtk_builder_get_object(sub_builder, "entry_col"));

  gtk_builder_connect_signals(sub_builder, NULL);
  gtk_window_set_position(GTK_WINDOW(sub_window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(sub_window), 400, 100);
  gtk_window_set_title(GTK_WINDOW(sub_window), "Generar");
  g_object_unref(sub_builder);
  gtk_widget_show_all(sub_window);

  gtk_main();
}

void lectura(){
  is_grabar = 0;
  gchar*      file_name;
  char str[MAXCHAR];
  int bt=0; int tp=0;
  int filter=0;
  char calc[4];//={0, 0, 0, 0};
  int n=0; int m=0;
  int x;
  int** matrix;
  int e=0;
  int calc_f=0;
  //free(matriz);

  file_name = gtk_file_chooser_get_filename (bt_fileCh);

  desc = fopen(file_name, "r");
  if(desc == NULL){
    printf("%s\n", "Error, no se puede abrir el archivo");
  }else{
    while (fgets(str, MAXCHAR, desc) != NULL){

        while(filter<2){
          if (str[tp++] ==  '_'){
            for(;bt<tp-1; bt++)calc[x++] = str[bt];
            if(filter==0)m = atoi(calc);
            if(filter==1)n = atoi(calc);
            bt++; tp++;
            filter++;
          }
        }
        e=tp-1;
        matriz = new_matrix(m, n);
        char scalc = str[e];
        int count=1;

        for (int i = 0; i < m; i++){
          for(int j=0; j< n; j++){

              if (str[e+1] == ' ') {
                calc_f = str[e]-'0';
                e+=2;
              }else if(str[e+2]==' '){
                calc_f = str[e]-'0'; calc_f*=10; calc_f += str[e+1]-'0';
                e+=3;
              }
              matriz[i][j]=calc_f;

          }
        }
      }
      fclose(desc);
    }

  sprintf(st1, "%d", m);
  sprintf(st2, "%d", n);
  gtk_label_set_text(GTK_LABEL(lb_m), st1);
  gtk_label_set_text(GTK_LABEL(lb_n), st2);
  rows = m; cols = n;
  //free(grid);
  }


void resolver(){
    GtkWidget *dialog;
    dialog=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"%s", "No implementado ):");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static gboolean clicked(GtkWidget *widget, GdkEventButton *event,
    gpointer user_data)
{

    if (event->button == 1) {

          mouse_x = event->x; mouse_y = event->y;
          gtk_widget_queue_draw(widget);
    }
    if (event->button == 2) {
        mouse_x =0; mouse_y=0; d_angle=0;
        gtk_widget_queue_draw(widget);
    }
    if (event->button == 3) {
        d_angle+=1.57;
        if(d_angle>=angle_top)d_angle=0;
        gtk_widget_queue_draw(widget);
    }

    return TRUE;
}


static gboolean scrolling(GtkWidget *widget, GdkEventButton *event,
    gpointer user_data)
{

    if(gdk_event_get_scroll_deltas (event, &scroll_handler.delta_x, &scroll_handler.delta_y)){

      scroll_y = -scroll_handler.delta_y;
      if(scaleFactor+scroll_y>.5 && scaleFactor+scroll_y<200)scaleFactor += scroll_y-.5;
      //printf("scaleFactor\t%f\n", scaleFactor);
      gtk_widget_queue_draw(widget);

    }


    return TRUE;
}

int main(int argc, char *argv[]){

  srand( (unsigned)time( NULL ) );
  //free(grid);
  //--------------------------------------------
  new_maze();
  //--------------------------------------------
  GtkBuilder*  builder;
  GtkWidget   *window;
  GtkWidget   *darea;

  gtk_init(&argc, &argv);
  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, "glade/window_main.glade", NULL);

  window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
  lb_resolver = GTK_WIDGET(gtk_builder_get_object(builder, "lb_resolver_prompt"));

  darea = GTK_WIDGET(gtk_builder_get_object(builder, "d_area"));
  bt_fileCh = GTK_WIDGET(gtk_builder_get_object(builder, "bt_file_chooser"));

  lb_n = GTK_WIDGET(gtk_builder_get_object(builder, "lb_n"));
  lb_m = GTK_WIDGET(gtk_builder_get_object(builder, "lb_m"));

  gtk_builder_connect_signals(builder, NULL);

//--------------------MOUSE-----HANLING-----------------
  gtk_widget_add_events(darea, GDK_BUTTON_PRESS_MASK);
  gtk_widget_add_events(window, GDK_SCROLL_MASK);
//-------------------------------------------------------

  g_signal_connect(G_OBJECT(darea), "draw",
      G_CALLBACK(on_draw_event), NULL);
//--------------------MOUSE-----HANLING--------------
  g_signal_connect(darea, "button-press-event",
      G_CALLBACK(clicked), NULL);
  g_signal_connect(window, "scroll-event",
        G_CALLBACK(scrolling), NULL);

//---------------------------------------------------

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), width, height);
  gtk_window_set_title(GTK_WINDOW(window), "mazeLab");

  g_object_unref(builder);
  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
