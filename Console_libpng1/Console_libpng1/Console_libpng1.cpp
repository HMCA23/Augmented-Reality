// Console_libpng1.cpp: define el punto de entrada de la aplicación de consola.
//

/*
 * Copyright 2002-2010 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */

//Héctor Miguel Camarillo Abad
//137754
//Realidad aumentada

//Todos los includes necesarios

#include "StdAfx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <cmath>  

#define PNG_DEBUG 3
#include <png.h>


/**
  * Aborta la operación cuando se llame el método
  * Ya programado en el ejemplo de libpng
  *
  * @author Guillaume Cottenceau
  * @param El mensaje que va a arrojar cuando aborte
  * @throws Abort

  */
void abort_(const char * s, ...)
{
        va_list args;
        va_start(args, s);
        vfprintf(stderr, s, args);
        fprintf(stderr, "\n");
        va_end(args);
        abort();
}//end abort

//Variables globales ya definidas en el ejemplo de libpng
int x, y;

int width, height;
png_byte color_type;
png_byte bit_depth;

png_structp png_ptr;
png_infop info_ptr;
int number_of_passes;
png_bytep * row_pointers;

FILE *fp;

/**
  * Lee una imagen de tipo png y la guarda con apuntadores
  * Ya programado en el ejemplo de libpng
  *
  * @author Guillaume Cottenceau
  * @param El título de la imagen a leer
  * @throws Aborts en caso de que:
  * El archivo no exista
  * El archivo no sea de tipo png

  */
void read_png_file(char* file_name)
{

		char header[8];    // 8 is the maximum size that can be checked

        /* open file and test for it being a png */
        FILE *fp = fopen(file_name, "rb");
        if (!fp)
                abort_("[read_png_file] File %s could not be opened for reading", file_name);
        fread(header, 1, 8, fp);
        if (png_sig_cmp(png_const_bytep(header), 0, 8))
                abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);


        /* initialize stuff */
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr)
                abort_("[read_png_file] png_create_read_struct failed");

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
                abort_("[read_png_file] png_create_info_struct failed");

        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[read_png_file] Error during init_io");

        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, 8);

        png_read_info(png_ptr, info_ptr);

        width = png_get_image_width(png_ptr, info_ptr);
        height = png_get_image_height(png_ptr, info_ptr);
        color_type = png_get_color_type(png_ptr, info_ptr);
        bit_depth = png_get_bit_depth(png_ptr, info_ptr);

        number_of_passes = png_set_interlace_handling(png_ptr);
        png_read_update_info(png_ptr, info_ptr);


        /* read file */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[read_png_file] Error during read_image");

        row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
        for (y=0; y<height; y++)
                row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

        png_read_image(png_ptr, row_pointers);

        fclose(fp);
}//end reading file

/**
  * Escribe la imagen modificada de tipo png dada por los apuntadores
  * Ya programado en el ejemplo de libpng
  *
  * @author Guillaume Cottenceau
  * @param El título con el que se guardará la imagen
  * @throws Aborts en caso de que:
  * Varios errores de la estructura png

  */
void write_png_file(char* file_name)
{
        /* create file */
	    int error = fopen_s(&fp, file_name, "wb");
        if (!fp)
                abort_("[write_png_file] File %s could not be opened for writing", file_name);

        /* initialize stuff */
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr)
                abort_("[write_png_file] png_create_write_struct failed");

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
                abort_("[write_png_file] png_create_info_struct failed");

        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during init_io");

        png_init_io(png_ptr, fp);


        /* write header */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during writing header");

        png_set_IHDR(png_ptr, info_ptr, width, height,
                     bit_depth, color_type, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        png_write_info(png_ptr, info_ptr);


        /* write bytes */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during writing bytes");

        png_write_image(png_ptr, row_pointers);


        /* end write */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during end of write");

        png_write_end(png_ptr, NULL);

        /* cleanup heap allocation */
        for (y=0; y<height; y++)
                free(row_pointers[y]);
        free(row_pointers);

        fclose(fp);
}//end write

/**
  * Escribe la imagen modificada de tipo png dada por los apuntadores
  * modificado a partir del ejemplo de libpng
  * única diferencia radica en que no libera la memoria de los apuntadores
  *
  * @author Original:Guillaume Cottenceau & Modificaciones:Héctor Miguel Camarillo Abad
  * @param El título con el que se guardará la imagen
  * @see write_png_file
  * @throws Aborts en caso de que:
  * Varios errores de la estructura png

  */
void write_middle_file(char* file_name)
{
        /* create file */
        //FILE *fp = fopen(file_name, "wb");
	    int error = fopen_s(&fp, file_name, "wb");
        if (!fp)
                abort_("[write_png_file] File %s could not be opened for writing", file_name);


        /* initialize stuff */
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr)
                abort_("[write_png_file] png_create_write_struct failed");

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
                abort_("[write_png_file] png_create_info_struct failed");

        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during init_io");

        png_init_io(png_ptr, fp);


        /* write header */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during writing header");

        png_set_IHDR(png_ptr, info_ptr, width, height,
                     bit_depth, color_type, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        png_write_info(png_ptr, info_ptr);


        /* write bytes */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during writing bytes");

        png_write_image(png_ptr, row_pointers);


        /* end write */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during end of write");

        png_write_end(png_ptr, NULL);

        fclose(fp);
} //end write middle

/**
  * Recorre toda la imagen leída, hace cero el canal rojo y el verde igual al azul
  *
  * @author Guillaume Cottenceau
  * @throws Aborts en caso de que:
  * El archivo png no contenga el canal alpha

  */
void process_file(void)
{
        if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
                abort_("[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA "
                       "(lacks the alpha channel)");

        if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA)
                abort_("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)",
                       PNG_COLOR_TYPE_RGBA, png_get_color_type(png_ptr, info_ptr));

        for (y=0; y<height; y++) {
                png_byte* row = row_pointers[y];
                for (x=0; x<width; x++) {
                        png_byte* ptr = &(row[x*4]);
                        printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n",x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

                        /* set red value to 0 and green value to the blue one */
                        ptr[0] = 0;
                        ptr[1] = ptr[2];
                }
        }
}//end proces file

/**
  * Recorre toda la imagen leída, convierte la imagen a
  * Escala de grises por medio de la fórmula de luminosity
  * modificado a partir del ejemplo de libpng
  * única diferencia radica en en procesamiento de la  imagen
  *
  * @author Original:Guillaume Cottenceau & Modificaciones:Héctor Miguel Camarillo Abad
  * @see process_file
  * @throws Aborts en caso de que:
  * El archivo png no contenga el canal alpha

  */
void my_process_gray(void)
{
        if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
                abort_("[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA "
                       "(lacks the alpha channel)");

        if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA)
                abort_("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)",
                       PNG_COLOR_TYPE_RGBA, png_get_color_type(png_ptr, info_ptr));

        for (y=0; y<height; y++) {
                png_byte* row = row_pointers[y];
                for (x=0; x<width; x++) {
                        png_byte* ptr = &(row[x*4]);

						//Se utiliza la fórmula de luminosity para encontral los valores
						ptr[0]=png_byte(0.21*ptr[0] + 0.72*ptr[1] + 0.07*ptr[2]);
						ptr[1]=ptr[0];
						ptr[2]=ptr[1];
                }
        }
} // end convierte a escala de gris

/**
  * Recorre toda la imagen leída, convierte la imagen a
  * sus contornos por medio de Sobel
  * modificado a partir del ejemplo de libpng
  * única diferencia radica en en procesamiento de la  imagen
  * Utiliza el método my_process_gray
  * 
  * @author Original:Guillaume Cottenceau & Modificaciones:Héctor Miguel Camarillo Abad
  * @see process_file & my_process_gray
  * @throws Aborts en caso de que:
  * El archivo png no contenga el canal alpha

  */
void my_process_contour(void)
{
		//Se define una matriz, donde se guardarán los valores de la imagen
		double **matrizImg;
		matrizImg = new double*[height]; //generando la matriz
		for(int i = 0; i < height; ++i)
		{
			matrizImg[i] = new double[width];
		}//End for de generar la matriz

        if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
                abort_("[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA "
                       "(lacks the alpha channel)");

        if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA)
                abort_("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)",
                       PNG_COLOR_TYPE_RGBA, png_get_color_type(png_ptr, info_ptr));

		//se llama a escala de grises
		my_process_gray();

		//acaban antes de recorrer toda la imagen para evitar que la
		//máscara se salga de rango
        for (y=0; y<height-1; y++) {
                for (x=0; x<width-1; x++) {
						//Definimos todos los valores de la máscara de 3x3
                        png_byte* ptr = &(row_pointers[y][x*4]);

						png_byte* ptr00 = &(row_pointers[abs(y-1)][abs(x-1)*4]);
						png_byte* ptr01 = &(row_pointers[abs(y-1)][abs(x-0)*4]);
						png_byte* ptr02 = &(row_pointers[abs(y-1)][abs(x+1)*4]);
						png_byte* ptr10 = &(row_pointers[abs(y-0)][abs(x-1)*4]);

						png_byte* ptr11 = &(row_pointers[y][x*4]);

						png_byte* ptr12 = &(row_pointers[abs(y-0)][abs(x+1)*4]);
						png_byte* ptr20 = &(row_pointers[abs(y+1)][abs(x-1)*4]);
						png_byte* ptr21 = &(row_pointers[abs(y+1)][abs(x-0)*4]);
						png_byte* ptr22 = &(row_pointers[abs(y+1)][abs(x+1)*4]);
						
						double Gx,Gy,G,Gt;

						//Se obtiene el gradiente en x por medio de Sobel
						Gx=-ptr00[0]+ptr02[0]-2*ptr10[0]+2*ptr12[0]-ptr20[0]+ptr22[0];
						//Se obtiene el gradiente en y por medio de Sobel
						Gy=-1*ptr00[0]-2*ptr01[0]-1*ptr02[0]+0*ptr10[0]+0*ptr11[0]+0*ptr12[0]+1*ptr20[0]+2*ptr21[0]+1*ptr22[0];
						//Se obtiene el gradiente en ambas direcciones
						G=abs(Gx+Gy)/2;

						//Se binariza el resultado del gradiente
						if(G>=127)
							Gt=0;
						else
							Gt=255;
						//El resultado se guarda en la matriz
						matrizImg[y][x]=Gt;
                }
        }
		//Se pasan los resultados de la matriz a la imágen
		for (y=0; y<height-1; y++) {
                for (x=0; x<width-1; x++) {
					png_byte* ptr = &(row_pointers[y][x*4]);
					ptr[0]=png_byte(matrizImg[y][x]);
					ptr[1]=ptr[0];
					ptr[2]=ptr[1];
				}
		}
}// end contornos

/**
  * Recorre toda la imagen leída, convierte la imagen a
  * sus esquinas basado en Harrys
  * modificado a partir del ejemplo de libpng
  * única diferencia radica en en procesamiento de la  imagen
  * Utiliza el método my_process_gray y el método my_process_contour
  * 
  * @author Original:Guillaume Cottenceau & Modificaciones:Héctor Miguel Camarillo Abad
  * @see process_file & my_process_gray & my_process_contour & write_middle_file
  * @throws Aborts en caso de que:
  * El archivo png no contenga el canal alpha

  */
void my_process_harrys(void)
{
		//Se declara la variable para contar 
		int ContadorEsquinas=0;
		//Se declara la matriz
		double **matrizImg;
		matrizImg = new double*[height]; //generando la matriz
		for(int i = 0; i < height; ++i)
		{
			matrizImg[i] = new double[width];
		}//End for de generar la matriz

        if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
                abort_("[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA "
                       "(lacks the alpha channel)");

        if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA)
                abort_("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)",
                       PNG_COLOR_TYPE_RGBA, png_get_color_type(png_ptr, info_ptr));

		//Escala de grises
		my_process_gray();
		write_middle_file("Escala_Grises.png");

		//Contorno
		my_process_contour();
		write_middle_file("Contornos.png");


        for (y=1; y<height-1; y++) {
                for (x=1; x<width-1; x++) {
					//Declaramos todas las posiciones de la imagen que vamos a ocupar
                        png_byte* ptr = &(row_pointers[y][x*4]);

						png_byte* ptr00 = &(row_pointers[abs(y-1)][abs(x-1)*4]);
						png_byte* ptr01 = &(row_pointers[abs(y-1)][abs(x-0)*4]);
						png_byte* ptr02 = &(row_pointers[abs(y-1)][abs(x+1)*4]);
						png_byte* ptr10 = &(row_pointers[abs(y-0)][abs(x-1)*4]);

						png_byte* ptr11 = &(row_pointers[y][x*4]);

						png_byte* ptr12 = &(row_pointers[abs(y-0)][abs(x+1)*4]);
						png_byte* ptr20 = &(row_pointers[abs(y+1)][abs(x-1)*4]);
						png_byte* ptr21 = &(row_pointers[abs(y+1)][abs(x-0)*4]);
						png_byte* ptr22 = &(row_pointers[abs(y+1)][abs(x+1)*4]);
						
						double Gx,Gy,Gt,G;
						//Declaramos un boolean que va a indicar si un pixel lo
						//Consideramos esquina o no
						bool esquina=false;
						//Se obtiene el gradiente en x y se saca el valor absoluto
						Gx=-ptr00[0]+ptr02[0]-2*ptr10[0]+2*ptr12[0]-ptr20[0]+ptr22[0];
						Gx=abs(Gx);
						//Se obtiene el gradiente en y y se saca el valor absoluto
						Gy=-1*ptr00[0]-2*ptr01[0]-1*ptr02[0]+0*ptr10[0]+0*ptr11[0]+0*ptr12[0]+1*ptr20[0]+2*ptr21[0]+1*ptr22[0];
						Gy=abs(Gy);
						G=(Gx+Gy)/2;
							
						//Una esquina produciría un gran cambio en todas las direcciones
						//Como en este programa los contornos son 0 y lo que no 255, entonces:
						if(G<=255&&int(ptr11[0])==0)
						{
							Gt=0;
							esquina=true;
						}
						else
							Gt=255;
						//Se le asigna el valor a la matriz
						//y a todos sus vecinos para que se note
						matrizImg[y][x]=Gt;

						if(esquina)
						{
							ContadorEsquinas++;
						}
                }
        }
		//Se pasan los valores a la imagen
		for (y=0; y<height-1; y++) {
				//printf("X is: %d Y is %d\n",x, y);
                for (x=0; x<width-1; x++) {
					png_byte* ptr = &(row_pointers[y][x*4]);
					ptr[0]=png_byte(matrizImg[y][x]);
					ptr[1]=ptr[0];
					ptr[2]=ptr[1];
				}
		}
		write_middle_file("esquinas.png");
}

/**
  *El programa recibe dos argumentos, el nombre del arrchivo que va a leer,
  *y el nombre que le pondrá al archivo procesado
  *en este caso el archivo debe llamarse Landscape_panshot
  *y el resultado es guardado como Landscape_panshot2

  */
int main(int argc, char **argv)
{
		if (argc != 3)
			abort_("Usage: program_name <file_in> <file_out>");	
		//Lee el archivo png
        read_png_file(argv[1]);
		//Realiza el procesado de la imagen
		my_process_harrys();
		//Escribe la imagen resultante
        write_png_file(argv[2]);
		//Termina el programa
        return 0;
}

