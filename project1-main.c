#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"
#define CHANNEL_NUM 3

// imatrix struct, wraps a byte array of pixel data for an image
typedef struct imatrix{
    int width;
    int height;

    uint8_t** r;
    uint8_t** g;
    uint8_t** b;
    struct imatrix* (*add)(struct imatrix* this, struct imatrix* m2);
    struct imatrix* (*subtract)(struct imatrix* this, struct imatrix* m2);
    struct imatrix* (*multiply)(struct imatrix* this, struct imatrix* m2);
    struct imatrix* (*scale)(struct imatrix* this, int width, int height, float alpha);
    void (*write_image_to_rgb)(struct imatrix* this);
    void (*write_rgb_to_image)(struct imatrix* m);
    struct imatrix* (*set_rgb_image)(struct imatrix* this, uint8_t* new_rgb_image, int width, int height, int channels);

    //internal image reference
    uint8_t* rgb_image;
} imatrix;

// initialize an imatrix, allocates all memory dynamically
imatrix* init_from_file(char* image_path, int* width, int* height, int* channels);
imatrix* init_from_rgb_image(uint8_t* rgb_image, int width, int height, int channels);
imatrix* init_blank_rgb_image(int width, int height, int channels);


// frees all allocated memory
void free_imatrix(imatrix* image);

enum RGB {RED, GREEN, BLUE};
void init_funcptrs(imatrix* this);
imatrix* init_rgb(imatrix* this, int width, int height, int channels);

imatrix* init_from_file(char* image_path, int* width, int* height, int* channels){
    int i,j;
    uint8_t* rgb_image;
    rgb_image = stbi_load(image_path, width, height, channels, CHANNEL_NUM);
    imatrix* image_matrix = malloc(sizeof(imatrix));

    init_funcptrs(image_matrix);
    image_matrix->rgb_image = rgb_image;
    image_matrix = init_rgb(image_matrix, *width, *height, CHANNEL_NUM);
    image_matrix->write_image_to_rgb(image_matrix);    

    return image_matrix;
}

imatrix* init_from_rgb_image(uint8_t* rgb_image, int width, int height, int channels){
    int i,j;

    if (rgb_image==NULL)
        return NULL;

    imatrix* image_matrix = malloc(sizeof(imatrix));
    init_funcptrs(image_matrix);
    image_matrix->rgb_image = rgb_image;
    image_matrix = init_rgb(image_matrix, width, height, CHANNEL_NUM);
    image_matrix->write_image_to_rgb(image_matrix);    

    return image_matrix;
}

imatrix* init_blank_rgb_image(int width, int height, int channels){
    int i,j;
    uint8_t*    rgb_image=NULL;

    imatrix* image_matrix = malloc(sizeof(imatrix));
    init_funcptrs(image_matrix);
    image_matrix->rgb_image = (uint8_t*) malloc(sizeof(uint8_t) * width * height * CHANNEL_NUM);
    image_matrix = init_rgb(image_matrix, width, height, CHANNEL_NUM);
    
    return image_matrix;
}

void init_funcptrs(imatrix* this){
    imatrix*    add(imatrix* m1, imatrix* m2); 
    imatrix*    subtract(imatrix* m1, imatrix* m2);
    imatrix*    multiply(imatrix* m1, imatrix* m2);
    void        write_image_to_rgb(imatrix* this);
    void        write_rgb_to_image(imatrix* m);
    imatrix*    scale(imatrix* this, int width, int height, float alpha);

    if (this==NULL)
        return;
    this->add = add;
    this->subtract = subtract;
    this->multiply = multiply;
    this->write_image_to_rgb = write_image_to_rgb;
    this->write_rgb_to_image = write_rgb_to_image;
    this->scale = scale;
}

imatrix* init_rgb(imatrix* this, int width, int height, int channels){
    int i,j;
    this->width = width;
    this->height = height;
    this->r = (uint8_t**)malloc(height * sizeof(uint8_t*));
    this->g = (uint8_t**)malloc(height * sizeof(uint8_t*));
    this->b = (uint8_t**)malloc(height * sizeof(uint8_t*));
    for (i = 0; i < height; i++){
        this->r[i] = (uint8_t*)malloc(width * sizeof(uint8_t));
        this->g[i] = (uint8_t*)malloc(width * sizeof(uint8_t));
        this->b[i] = (uint8_t*)malloc(width * sizeof(uint8_t));
        }
    // YOUR CODE HERE
    return this;
}

imatrix* set_rgb_image(imatrix* this, uint8_t* new_rgb_image, int height, int width, int channels){
    free_imatrix(this);
    return init_from_rgb_image(new_rgb_image, height, width, channels);
}




/*
*   Add two matrices together.  Creates a new imatrix object and returns a referecne to it.  Returns
*   NULL if the matrices have different sizes.
*
*   @param m1 the imatrix object for the first image
*   @param m2 the imatrix object for the second image
*   @returns a reference to a newly allocated imatrix object that is the clipped sum of the two input images or
*                NULL if the matrices are not the same size (same number of rows and columns).
*            Note: This memory must be freed when you're done using it.
*/
imatrix* add(imatrix* m1, imatrix* m2){

    if (m1->height != m2->height||m1->width != m2->width){
    printf("Null");
    return NULL;
    }

    printf("Not Null");
    int height = m1->height;
    int width = m1->width;
    
    imatrix* addMatrix = init_blank_rgb_image(width, height, 3);

        for (int i=0; i<height; i++){
            for (int j=0; j<width; j++){
                if((m1->r[i][j] + m2->r[i][j])>=255){
                    addMatrix->r[i][j] = 255;
                }else
                addMatrix->r[i][j] = m1->r[i][j] + m2->r[i][j];

                if((m1->g[i][j] + m2->g[i][j])>=255){
                    addMatrix->g[i][j] = 255;
                }else
                addMatrix->g[i][j] = m1->g[i][j] + m2->g[i][j];

                if((m1->b[i][j] + m2->b[i][j])>=255){
                    addMatrix->b[i][j] = 255;
                }else
                addMatrix->b[i][j] = m1->b[i][j] + m2->b[i][j];
            }
        }
    addMatrix->write_rgb_to_image(addMatrix);
    //          for (int j=0; j<width; j++){
    //             // if((m1->r[i][j] + m2->r[i][j]) > 255 ){
    //             //     addimage->r[i][j] = m1->r[i][j] + m2->r[i][j] - 255;
    //             // }
    //             // else{
    //             //     addimage->r[i][j] = m1->r[i][j] + m2->r[i][j];
    //             // }

    //             // if((m1->g[i][j] + m2->g[i][j]) > 255){
    //             //     addimage->g[i][j] = m1->g[i][j] + m2->g[i][j] - 255;
    //             // }
    //             // else{
    //             //     addimage->g[i][j] = m1->g[i][j] + m2->g[i][j];
    //             // }

    //             // if((m1->b[i][j] + m2->b[i][j]) > 255){
    //             //     addimage->b[i][j] = m1->b[i][j] + m2->b[i][j] - 255;
    //             // }
    //             // else{
    //             //     addimage->b[i][j] = m1->b[i][j] + m2->b[i][j];
    //             // }
                
    //             addimage ->r[i][j]= 125;
    //             addimage -> g[i][j] = 125;
    //             addimage -> b [i][j]= 125;



                
    //          }
    // }
    

    // int height = m1->height, width = m1->width;
    // imatrix* subMatrix = init_blank_rgb_image(width, height, 3);
    //     for (int i=0; i<height; i++){
    //         for (int j=0; j<width; j++){
    //             if(m1->r[i][j] < m2->r[i][j]){
    //                 subMatrix->r[i][j] = 0;
    //             }else
    //             subMatrix->r[i][j] = m1->r[i][j] - m2->r[i][j];

    //             if(m1->g[i][j] < m2->g[i][j]){
    //                 subMatrix->g[i][j] = 0;
    //             }else
    //             subMatrix->g[i][j] = m1->g[i][j] - m2->g[i][j];

    //             if(m1->b[i][j] < m2->b[i][j]){
    //                 subMatrix->b[i][j] = 0;
    //             }else
    //             subMatrix->b[i][j] = m1->b[i][j] - m2->b[i][j];
    //         }
    //     }
    // subMatrix->write_rgb_to_image(subMatrix);
    return addMatrix;

    // if (m1->height != m2->height||m1->width != m2->width)
    // return NULL;

    // int height = m1->height, width = m1->width;
    // imatrix* addMatrix = init_blank_rgb_image(width, height, 3);
    //     for (int i=0; i<height; i++){
    //         for (int j=0; j<width; j++){
    //             if((m1->r[i][j] + m2->r[i][j])>=255){
    //                 addMatrix->r[i][j] = m1->r[i][j] + m2->r[i][j]-255;
    //             }else
    //             addMatrix->r[i][j] = m1->r[i][j] + m2->r[i][j];

    //             if((m1->g[i][j] + m2->g[i][j])>=255){
    //                 addMatrix->g[i][j] = m1->r[i][j] + m2->g[i][j]-255;
    //             }else
    //             addMatrix->g[i][j] = m1->g[i][j] + m2->g[i][j];

    //             if((m1->b[i][j] + m2->b[i][j])>=255){
    //                 addMatrix->b[i][j] = m1->b[i][j] + m2->b[i][j]-255;
    //             }else
    //             addMatrix->b[i][j] = m1->b[i][j] + m2->b[i][j];
    //         }
    //     }
    // addMatrix->write_rgb_to_image(addMatrix);
    // return addMatrix;

    // YOUR CODE HERE
}


/*
*   Subtract m2 from m1.  Creates a new imatrix object and returns a referecne to it. Returns
*   NULL if the matrices have different sizes.
*
*   @param m1 the imatrix object for the first image
*   @param m2 the imatrix object for the second image
*   @returns a reference to a newly allocated imatrix object that is the clipped sum of the two input images or
*                NULL if the matrices are not the same size (same number of rows and columns).
*            Note: This memory must be freed when you're done using it.
*/
imatrix* subtract(imatrix* m1, imatrix* m2){

     if (m1->height != m2->height||m1->width != m2->width)
    return NULL;
    

    int height = m1->height;
    int width = m1->width;
    
    imatrix* subimage = init_blank_rgb_image(width, height, 3);

    for (int i=0; i<height; i++){
             for (int j=0; j<width; j++){
                if((m1->r[i][j] - m2->r[i][j]) <= 0){
                    subimage->r[i][j] = m1->r[i][j] - m2->r[i][j] + 255;
                }
                else{
                    subimage->r[i][j] = m1->r[i][j] - m2->r[i][j];
                }

                if((m1->g[i][j] - m2->g[i][j]) <= 0){
                    subimage->g[i][j] =  m1->g[i][j] - m2->g[i][j] + 255;
                }
                else{
                    subimage->g[i][j] = m1->g[i][j] - m2->g[i][j];
                }

                if((m1->b[i][j] - m2->b[i][j]) <= 0){
                    subimage->b[i][j] = m1->b[i][j] - m2->b[i][j] + 255;
                }
                else{
                    subimage->b[i][j] = m1->b[i][j] - m2->b[i][j];
                }
                



                
             }
    }
    
    subimage->write_rgb_to_image(subimage);

    // int height = m1->height, width = m1->width;
    // imatrix* subMatrix = init_blank_rgb_image(width, height, 3);
    //     for (int i=0; i<height; i++){
    //         for (int j=0; j<width; j++){
    //             if(m1->r[i][j] < m2->r[i][j]){
    //                 subMatrix->r[i][j] = 0;
    //             }else
    //             subMatrix->r[i][j] = m1->r[i][j] - m2->r[i][j];

    //             if(m1->g[i][j] < m2->g[i][j]){
    //                 subMatrix->g[i][j] = 0;
    //             }else
    //             subMatrix->g[i][j] = m1->g[i][j] - m2->g[i][j];

    //             if(m1->b[i][j] < m2->b[i][j]){
    //                 subMatrix->b[i][j] = 0;
    //             }else
    //             subMatrix->b[i][j] = m1->b[i][j] - m2->b[i][j];
    //         }
    //     }
    // subMatrix->write_rgb_to_image(subMatrix);
    return subimage;
        // YOUR CODE HERE
}

/*
*   Matrix multiplication.  Multiplies m1*m2 using matrix-matrix multiply.  Returns a new imatrix object with
*   the output.  If m1 is a (m, n) matrix, then m2 must be a (n, k) matrix for any value of k >=1.
* 
*   @param m1 the left matrix.
*   @param m2 the right matrix.
*   @returns a new matrix with the result or NULL of the dimensions do not match properly for matrix multiplications.  
*               Note: This memory must be freed after use.
*/
imatrix* multiply(imatrix* m1, imatrix* m2){
    
    if (m1->height != m2->width|| m2->width < 1.0)
        return NULL;

    printf("m1 has width %d and height %d\n", m1->width, m1->height);
    printf("m2 has width %d and height %d\n", m2->width, m2->height);
    
    int height = m1->height;
    int width = m2->width;
    

    
    imatrix* multimage = init_blank_rgb_image(width, height, 3);
    

    printf("width = %d, height = %d", multimage->width,multimage->height);

    int r1 = m1->height;
    int c1  = m1->width;
    int c2 = m2->width;

    for (int i = 0; i < r1; ++i) {
      for (int j = 0; j < c2; ++j) {
         for (int k = 0; k < c1; ++k) {
            multimage->r[i][j] += m1->r[i][k] * m2->r[k][j];
         }
      }
   }
   for (int i = 0; i < r1; ++i) {
      for (int j = 0; j < c2; ++j) {
         for (int k = 0; k < c1; ++k) {
            multimage->g[i][j] += m1->g[i][k] * m2->g[k][j];
         }
      }
   }
   for (int i = 0; i < r1; ++i) {
      for (int j = 0; j < c2; ++j) {
         for (int k = 0; k < c1; ++k) {
            multimage->b[i][j] += m1->b[i][k] * m2->b[k][j];
         }
      }
   }

    
    multimage->write_rgb_to_image(multimage);
    
    return multimage;
}

/*
*   Scales all the pixel values by alpha for all alpha values between 0.0 and 1.0.  If the values are invalid, the references
*   to the input image is returned without modification.
* 
*   @param this input matrix.
*   @returns this with all the pixel values scaled by alpha.
*/
imatrix* scale(imatrix* this, int width, int height, float alpha){
    
    if(alpha >= 1.0 || alpha <= 0 ){
        return this;
    }

    
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            this->r[i][j] = alpha*this->r[i][j];
            this->g[i][j] = alpha*this->g[i][j];
            this->b[i][j] = alpha*this->b[i][j];
        }
    }

    

    // for (int k=0; k<3; k++){
    //     for (int i=0; i<height; i++){
    //         for (int j=0; j<width; j++){
    //             if (k == 0){
    //                 this->r[i][j] *= alpha;
    //             }else if(k == 1){
    //                 this->g[i][j] *= alpha;
    //             }else{
    //                 this->b[i][j] *= alpha;
    //             }

    //         }
    //     }
    // }
    return this;
    // YOUR CODE HERE
}

void write_rgb_to_image(imatrix* m){
    int i,j, height, width;
    height = m->height;
    width = m->width;

    for (i=0; i<height; ++i){
        for(j=0; j<width; ++j){
            *( m->rgb_image + (i * (CHANNEL_NUM * width) + (CHANNEL_NUM * j)) + RED) = m->r[i][j];
            *( m->rgb_image + (i * (CHANNEL_NUM * width) + (CHANNEL_NUM * j)) + GREEN) = m->g[i][j];
            *( m->rgb_image + (i * (CHANNEL_NUM * width) + (CHANNEL_NUM * j)) + BLUE) = m->b[i][j];
        }
    }
}

void write_image_to_rgb(imatrix* m){
    int i,j, height, width;
    height = m->height;
    width = m->width;

    for (i=0; i<height; ++i){
        for(j=0; j<width; ++j){
            m->r[i][j] = *( m->rgb_image + (i * (CHANNEL_NUM * width) + (CHANNEL_NUM * j)) + RED);
            m->g[i][j] = *( m->rgb_image + (i * (CHANNEL_NUM * width) + (CHANNEL_NUM * j)) + GREEN);
            m->b[i][j] = *( m->rgb_image + (i * (CHANNEL_NUM * width) + (CHANNEL_NUM * j)) + BLUE);
        }
    }
}

void free_imatrix(imatrix* image_matrix){
    int i;
    if (image_matrix==NULL)
        return;

    for (i=0; i<image_matrix->height; ++i){
        free(*(image_matrix->r +i));
        free(*(image_matrix->g + i));
        free(*(image_matrix->b + i));
    }
    free(image_matrix->r);
    free(image_matrix->g);
    free(image_matrix->b);

    stbi_image_free(image_matrix->rgb_image);
}

int main(int argc,  char* argv[]) {
    int width, height, channels;
    int width2, height2, channels2;
    int i;
    uint8_t* rgb_image;
    
    for (i=1; i<argc; ++i)
        printf("%d:\t%s\n", i, argv[1]);

    // Load an image
    imatrix* M = init_from_file("images/redsquare.png", &width, &height, &channels);
    if (M != NULL)
        stbi_write_png("images/image.png", width, height, CHANNEL_NUM, M->rgb_image, width*CHANNEL_NUM);

    //Load another image
    imatrix* M2 = init_from_file("images/blue.png", &width2, &height2, &channels2);
    if (M2 != NULL) 
        stbi_write_png("images/image2.png", width2, height2, CHANNEL_NUM, M2->rgb_image, width2*CHANNEL_NUM);
    

    
    //resize the image to the dimensions of the first image
    uint8_t* output_pixels = (uint8_t*)malloc(width*height*CHANNEL_NUM*sizeof(uint8_t));
    stbir_resize_uint8(M2->rgb_image , width2 , height2 , 0,
                        output_pixels, width, height, 0,
                                     CHANNEL_NUM);
    stbi_write_png("images/image3.png", width, height, CHANNEL_NUM, output_pixels, width*CHANNEL_NUM);
    

    printf("loaded");

    printf("M.r = %d\n", M->r[1][1]);
    printf("M2.r = %d\n", M2->r[1][1]);
    printf("M.b = %d\n", M->b[1][1]);
    printf("M2.b = %d\n", M2->b[1][1]);

    //add two images
    imatrix* M2_resized = init_from_rgb_image(output_pixels, width, height, channels);
    imatrix* sum_image = M2_resized->add(M2_resized, M);
    stbi_write_png("images/sum.png", width, height, CHANNEL_NUM, sum_image->rgb_image, width*CHANNEL_NUM);

    printf("sum_image.r = %d\n", sum_image->r[1][1]);
    
    printf("sum_image.b = %d\n", sum_image->b[1][1]);
    
    
    printf("added");
    
    //subtract them
    imatrix* diff_image = sum_image->subtract(sum_image, M);
    stbi_write_png("images/diff.png", width, height, CHANNEL_NUM, diff_image->rgb_image, width*CHANNEL_NUM);

    
    //scale then add
    float alpha = 0.4;

    
    M2_resized->scale(M2_resized, width, height, alpha);
    
    M->scale(M, width, height, (1-alpha));
    
    imatrix* scaled_sum = M2_resized->add(M2_resized, M);
    
    stbi_write_png("images/scaled_sum.png", width, height, CHANNEL_NUM, scaled_sum->rgb_image, width*CHANNEL_NUM);
    

    printf("before multiply");
    //scaled and translate (matrix multiply)
    // stbir_resize_uint8(M2->rgb_image , height2 , width2 , 0,
    //                     output_pixels, width, height, 0,
    //                                  CHANNEL_NUM);
    imatrix* res_image = M2_resized->multiply(M2_resized, M);
    
    stbi_write_png("images/multiply.png", width, height, CHANNEL_NUM, res_image->rgb_image, width*CHANNEL_NUM);
    
    
    // OPTIONAL: Test code for matrix multiply
    printf("after multiply");
    
    // free memory
    free_imatrix(M);
    free_imatrix(M2);
    free_imatrix(M2_resized);
    free_imatrix(sum_image);
    free_imatrix(diff_image);
    free_imatrix(res_image);

    return 0;
}
