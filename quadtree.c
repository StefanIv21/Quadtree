#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

//strcutra pentru a retine culorile fiecarui pixel
typedef struct pixel {
    unsigned char red, green, blue;
}pixel;

//stuctura pentru a crea arborele
typedef struct Tree {
	unsigned char red, green, blue; 
	uint32_t area;
	struct Tree *top_left, *top_right;
	struct Tree *bottom_left, *bottom_right;
 }Tree, *Nod;

typedef struct QuadtreeNode {
	unsigned char blue, green, red;	
	uint32_t area;
	int32_t top_left,   top_right;
	int32_t bottom_left, bottom_right;
 }__attribute__((packed)) QuadtreeNode;

//functie pentru a citi arborele
void compresie(pixel ** matrice, Nod * tree, uint32_t x, uint32_t y, uint32_t size, uint32_t prag, uint32_t *nr_noduri)
{
	uint32_t i, j;
	unsigned long long int blue = 0, green = 0, red = 0, mean = 0;
	Nod nod = malloc(sizeof(Tree));
	nod->area = size*size;//pun dimensiunea actuala
	nod->top_right    = NULL;
	nod->top_left     = NULL;
	nod->bottom_left  = NULL;
	nod->bottom_right = NULL;
    (*nr_noduri)++;
	//calculez blue ,green, red
	for(i = x; i < x + size; i++)
	{
		for(j = y; j < y + size; j++)
		{
			blue  = blue  + matrice[i][j].blue;
			green = green + matrice[i][j].green;
			red   = red   + matrice[i][j].red;
		}
	}
	blue = blue / (size * size);
	nod->blue = blue;
	red = red / (size * size);
	nod->red = red;
	green = green / (size * size);
	nod->green = green;
	//calculez mean confrom formulei date
	for(i = x; i < x + size; i++)
	{
		for(j = y; j < y + size; j++)
		{	
			mean = mean + ((red - matrice[i][j].red)     * (red - matrice[i][j].red)) + ((green - matrice[i][j].green) * (green - matrice[i][j].green)) + ((blue - matrice[i][j].blue)   * (blue - matrice[i][j].blue));
		}
	}
	mean = mean / (3 * size * size);
    *tree = nod;//pun fiecare nod creat in arbore
	Nod copie = *tree;
	//parcurg recursiv
	if(mean > prag)
	{
		compresie(matrice, &copie->top_left, x, y, size/2, prag, nr_noduri);
		compresie(matrice, &copie->top_right, x , y+ (size/2), size/2, prag, nr_noduri);
		compresie(matrice, &copie->bottom_right, x + (size/2), y + (size/2), size/2, prag, nr_noduri);
		compresie(matrice, &copie->bottom_left, x + (size/2), y , size/2, prag, nr_noduri);
	}
}
//functie care pune datele din arbore in vector
void vector(Nod tree, QuadtreeNode *v, uint32_t nr_nod, uint32_t *nr_culori) 
{
	Nod coada[nr_nod];//vector de tipul Nod  care retine adresele nodurilor din arbore
	int curent = 0; //variabila in care retin indicele vectorului
	int  next = 0; // variabila in care numerotez descendenti unui nod
	coada[0] = tree;
	while ( nr_nod )
	{
		//pun culorile fiecarui nod
		v[curent].red=coada[curent]->red;
		v[curent].green=coada[curent]->green;
		v[curent].blue=coada[curent]->blue;
		v[curent].area = coada[curent]->area;
		//daca nodul este frunza ,nu mai are descendenti
		if (coada[curent]->top_left == NULL )
		{
			v[curent].top_left = -1;
			v[curent].top_right = -1;
			v[curent].bottom_right = -1;
			v[curent].bottom_left = -1;
			(*nr_culori)++;
		}
		else
		{
			//numerote descendetii si retin adresele nodurilor urmatoare
            next++;
		    v[curent].top_left = next;
			next++;
			v[curent].top_right = next;
			next++;
			v[curent].bottom_right = next;
			next++;
			v[curent].bottom_left = next;
			coada[next-3] = coada[curent] -> top_left;
			coada[next-2] = coada[curent] -> top_right;
			coada[next-1] = coada[curent] -> bottom_right;
			coada[next] = coada[curent] -> bottom_left;
		}
		++curent;
		nr_nod--;
	}
}
void orizontal(Nod  *tree)
{
	Nod copie = *tree;
	if(copie->top_left != NULL )
	{
		//parcurg recursiv pana ajung la parintele unui nod frunza
		orizontal (&copie->top_left);
		orizontal (&copie->top_right);
		orizontal (&copie->bottom_right);
		orizontal (&copie->bottom_left);
	}
	Nod aux;//nod auxiliar ce retine temporar adresa unui  descendet pentru a face schimbul intre noduri
	aux  = copie->top_left;
	copie->top_left  = copie->top_right;
	copie->top_right = aux;
	aux	 = copie->bottom_left;
	copie->bottom_left = copie->bottom_right;
	copie->bottom_right = aux;
	
}
void vertical(Nod * tree)
{
	Nod copie=*tree;
	if(copie->top_left != NULL )
	{
		//parcurg recursiv pana ajung la parintele unui nod frunza
		vertical(&copie->top_left);
		vertical(&copie->top_right);
		vertical(&copie->bottom_right);
		vertical(&copie->bottom_left);
	}
	Nod  aux;//nod auxiliar ce retine temporar adresa unui  descendet pentru a face schimbul intre noduri
	aux  = copie->top_left;
	copie->top_left  = copie->bottom_left;
	copie->bottom_left = aux;
	aux	 = copie->top_right;
	copie->top_right = copie->bottom_right;
	copie->bottom_right = aux;
	
}
//functie recursiva pentru a construi matricea  avand arborele de compresie
void decompresie(Nod tree, pixel  *matrice[], uint32_t x, uint32_t y, uint32_t size)
{
	uint32_t i, j;
    //daca nodul este frunz pun culorile in matrice
	if(tree->top_left == NULL)
	{
		for(i = x; i < x + size; i++)
		{
			for(j = y; j < y + size; j++)
			{
				matrice[i][j].red   = tree->red;
				matrice[i][j].green = tree->green;
				matrice[i][j].blue  = tree->blue;
			}
		}
	}
	else
	{
		//parcurg recursiv pana ajung la un  nod frunza
		decompresie(tree->top_left, matrice, x, y, size/2);
		decompresie(tree->top_right, matrice, x, y+(size/2), size/2);
		decompresie(tree->bottom_right, matrice, x+(size/2), y+(size/2), size/2);
		decompresie(tree->bottom_left, matrice, x+(size/2), y, size/2);
	}

}
//functie pentru eliberarea memoriei
void free_tree ( Nod root )
{
	if ( root->top_left != NULL )
	{
		free_tree ( root -> top_left );
		free_tree ( root -> top_right );
		free_tree ( root -> bottom_right );
		free_tree ( root -> bottom_left );	
	}
	free ( root );
}
void tree(QuadtreeNode * vec, Nod *node, int i)
{
	Nod nod = malloc(sizeof(Tree));
	nod->red   = vec[i].red;
	nod->blue  = vec[i].blue;
	nod->green = vec[i].green;
	nod->area  = vec[i].area;
	*node = nod;
	Nod copie = *node;

	if(vec[i].top_left != -1 )
	{
		tree(vec, &(copie)->top_left,     vec[i].top_left);
		tree(vec, &(copie)->top_right,    vec[i].top_right);
		tree(vec, &(copie)->bottom_left,  vec[i].bottom_left);
		tree(vec, &(copie)->bottom_right, vec[i].bottom_right);

	}
	else
	{
		copie->top_left     = NULL;
		copie->top_right    = NULL;
		copie->bottom_left  = NULL;
		copie->bottom_right = NULL;
	}

}
void scrie(pixel ** imag, char * file, int width)
{
	FILE * f = fopen(file, "w");
	fprintf(f, "P6\n");
	fprintf(f, "%d %d\n", width, width );
	fprintf(f, "255\n");
	for(int i = 0; i < width; i++)
		fwrite(imag[i], sizeof(pixel), width, f);
	fclose(f);
}
int main(int argc, char * argv[])
{
	if (strstr(argv[1], "-c") )
	{
		uint32_t i;
		uint32_t width, height;//variabile pentru a lua lungimea si latimea unei imagini
		Nod tree = NULL; 
		char nume[3];// variabila in care pun tipul fisierului PPM
		uint32_t color;//variabila unde iau valoarea maxima pe care o poate lua o culoare
		FILE*FP = fopen(argv[3], "rb");
		if(!FP)
		{
			printf("nu");
		}
		//citesc din fisier datele si le pun in variabile 
		fread(nume, sizeof(char), 2, FP);
		nume[2] = '\0';
		fscanf(FP, "%d "  ,&width);
		fscanf(FP, "%d\n" ,&height);
		fscanf(FP, "%d"   ,&color);
		char chestie;
		fread(&chestie, sizeof(char) ,1 , FP);
		//creez o matrice de tip pixel(care retine culorile RGB pentru fiecare pixel)
		pixel **imag = malloc(sizeof(pixel*) * height);
		//aloc memorie
		for(i = 0; i < height; i++)
		{	
			imag[i] = malloc(sizeof(pixel) * width);
		}
		//citesc din fisier culorile RGB si le pun in matrice
		for (i = 0; i < width; i++)
		{
			for(int j = 0; j < height; j++)
			{
				fread(&imag[i][j], sizeof(pixel), 1, FP);
			}
		}
		fclose(FP);
		uint32_t numar = atoi(argv[2]); //iau mean-ul din argument
		uint32_t nr_noduri = 0;//variabila in care retin numarul nodurilor
		compresie(imag, &tree, 0, 0, width,numar,&nr_noduri);//construiesc vectorul de compresie
		QuadtreeNode *v = NULL; //vectorul pentru compresie
		uint32_t nr_culori = 0;//numar de culori
		//creez vectorul de tip QuadtreeNode si aloc memorie
		v = malloc(nr_noduri * sizeof(QuadtreeNode)); 
		vector(tree, v, nr_noduri, &nr_culori); //construiesc arborele de compresie
		//deschid fisierul si pun datale cerute in fisier
		FILE* f = fopen(argv[4], "wb");
		fwrite(&nr_culori, sizeof(uint32_t), 1, f); 
		fwrite(&nr_noduri, sizeof(uint32_t), 1, f);
		fwrite(v, sizeof(QuadtreeNode), nr_noduri, f);
		//eliberez memoria
		for ( i = 0; i < height; ++i )
		{
			free ( imag[i] );
		}
		free ( imag );
		free ( v );
		free_tree( tree );
		fclose(f);
	}
	if (strstr(argv[1], "-m") )
	{
		uint32_t i,j;
		uint32_t width, height;//variabile pentru a lua lungimea si latimea unei imagini
		Nod tree = NULL;
		char nume[3];// variabila in care pun tipul fisierului PPM
		uint32_t color;//variabila unde iau valoarea maxima pe care o poate lua o culoare
		FILE *FP = fopen(argv[4], "rb");
		if(!FP)
		{
			printf("nu");
		}
		//citesc din fisier datele si le pun in variabile 
		fread(nume, sizeof(char) ,2 , FP);
		nume[2] = '\0';
		fscanf(FP, "%d "  ,&width);
		fscanf(FP, "%d\n" ,&height);
		fscanf(FP, "%d"   ,&color);
		char chestie;
		fread(&chestie, sizeof(char), 1, FP);
		//creez o matrice de tip pixel(care retine culorile RGB pentru fiecare pixel)
		//aloc memorie
		pixel **imag = malloc(sizeof(pixel*) * height);
		for(i = 0; i < height; i++)
		{	
			imag[i] = malloc(sizeof(pixel) * width);
		}
		//citesc din fisier culorile RGB si le pun in matrice
		for (i = 0; i < width; i++)
		{
			for(int j = 0; j < height; j++)
			{
				fread(&imag[i][j], sizeof(pixel), 1, FP);
			}
		}
		fclose(FP);
		uint32_t numar = atoi(argv[3]); //iau mean-ul din argument
		uint32_t nr_noduri = 0;//numar de culori
		compresie(imag, &tree, 0, 0, width, numar, &nr_noduri);  //construiesc arborele de compresie
		//verific tipul de rotire a imaginii
		if(strcmp(argv[2],"h") == 0)
			{
				orizontal(&tree);
			}
		if(strcmp(argv[2], "v") == 0)
			{
				vertical(&tree);
			}
		pixel **a = malloc(sizeof(pixel*) * height);
		for(i = 0; i < height; i++)
		{	
			a[i] = malloc(sizeof(pixel) * width);
		}
		decompresie(tree,a,0,0,width); // Construiesc matricea de pixeli din arbore
		//deschid fisierul si pun datale cerute in fisier
		FILE *f = fopen(argv[5], "wb");
		fprintf(f, "P6\n");
		fprintf(f, "%d %d\n", width, width);
		fprintf(f, "%d\n",color);
		for (i = 0; i < width; i++)
		{
			for ( j = 0; j < width; ++j )
			{
				fwrite(&a[i][j], sizeof(pixel), 1, f);
			}
		}
		//eliberez memorie
		for(i = 0; i < width; i++)
		{
			free(imag[i]);
		}
		free(imag);
		for(i = 0; i < height; i++)
		{
			free(a[i]);
		}
		free(a);
		free_tree ( tree );
		fclose(f);
		}
	if (strstr(argv[1], "-d"))
		{
			
			unsigned int nr;
			unsigned int colors;
			FILE * f = fopen(argv[2], "rb");
			fread(&colors, sizeof(int), 1, f);
			fread(&nr,  sizeof(int), 1, f);			
			QuadtreeNode * vec = malloc(sizeof(QuadtreeNode) * nr);
			fread(vec, sizeof(QuadtreeNode), nr, f);
			fclose(f);

			Nod node = NULL;
			tree(vec, &node, 0);
			free(vec);
			uint32_t width = sqrt(node->area);
			uint32_t height =width;
								
			pixel **a = malloc(sizeof(pixel*) * height);
			for(int i = 0; i < height; i++)
			{	
			a[i] = malloc(sizeof(pixel) * width);
			}
			
			decompresie(node, a, 0, 0, width);
			scrie(a, argv[3], width);

		
			for(int i = 0; i < width; i++)
				free(a[i]);
			free(a);

		}
	return 0;
}
