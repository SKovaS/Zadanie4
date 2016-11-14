/**
 * Copies a BMP piece by piece, just because.
 */
 #include <stdio.h>
#include <stdlib.h>
 
#include "bmp.h"
 
int main(int argc, char* argv[])
{
    // Проверяем наличие правильного числа аргументов;
    if (argc != 4)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }
 
    // Вводим число с плавающей запятой. Оно и будет служить коэфициентом маштабирования.
    float factor = atof(argv[1]);
    
    // Вносим имена входящего и исхожящего файла.
    char* infile = argv[2];
    char* outfile = argv[3];
 
    // Открываем файл ввода.
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Opening impossible %s.\n", infile);
        return 2;
    }
 
    // Открываем файл вывода.
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Creation can not be %s.\n", outfile);
        return 3;
    }
 
    // Считываем файл BITMAPFILEHEADER.
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
 
    // Считываем файл BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
 
    // Проверяем файл ли 24 битный ВМР.
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
 
    // Определяем линии развертки изображения.
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // Храним в масиве весь набор пикселей изображения. 
    RGBTRIPLE image[bi.biWidth][abs(bi.biHeight)];
    
    
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // Перебираем пиксели развертки.
        for (int j = 0; j < bi.biWidth; j++)
        {
            RGBTRIPLE triple;
 
            // Считываем RGB.
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            
            // Заносим в масив.
            image[j][i] = triple;
        }
        

        fseek(inptr, padding, SEEK_CUR);
    }
    
    // Изменяем шырину и высоту с новыми параметрами.
    bi.biWidth =bi.biWidth * factor;
    bi.biHeight =bi.biHeight * factor;
    
    // Пересчитываем отступы.
    padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // Вычисляем число байтов в не заголовка.
    bi.biSizeImage = abs(bi.biHeight) * (bi.biWidth * sizeof(RGBTRIPLE) + padding);
    
    // Вычисляем число байтов вместе с заголовком.
    bf.bfSize = bi.biSizeImage + 54;
    
    // Пишем выходной файл BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
 
    // Пишем выходной файл BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
    
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        for (int j = 0; j < bi.biWidth; j++)
        {
            RGBTRIPLE triple;
            
            // Получаем пиксель который мы поместим используя коэфициент масшабирования.
            triple = image[(int) (j / factor)][(int) (i / factor)];
            
            
            // Записываем пиксель в файл.
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr); 
        }
        
        // Пишем в буфер.
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }
    
 
    // close infile
    fclose(inptr);
 
    // close outfile
    fclose(outptr);
 
    // that's all folks
    return 0;
}