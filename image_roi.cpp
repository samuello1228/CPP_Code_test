#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
using namespace std;

int main(int argc, char *argv[])
{
    // check the number of argument
    if (argc != 7)
    {
        cout << "The number of argument should be 6." << endl;
        cout << "image_roi.exe <image file name> <width of image> <roi_startx> <roi_starty> <roi_width> <roi_height>" << endl;
        return 0;
    }

    // check all parameters
    const string image_file_name = argv[1];

    char *pEnd;
    const long int width_of_image = strtol(argv[2], &pEnd, 10);
    if (*pEnd || argv[2][0] == '\0')
    {
        cout << "<width of image> should be an integer." << endl;
        return 0;
    }

    const long int roi_startx = strtol(argv[3], &pEnd, 10);
    if (*pEnd || argv[3][0] == '\0')
    {
        cout << "<roi_startx> should be an integer." << endl;
        return 0;
    }

    const long int roi_starty = strtol(argv[4], &pEnd, 10);
    if (*pEnd || argv[4][0] == '\0')
    {
        cout << "<roi_starty> should be an integer." << endl;
        return 0;
    }

    const long int roi_width = strtol(argv[5], &pEnd, 10);
    if (*pEnd || argv[5][0] == '\0')
    {
        cout << "<roi_width> should be an integer." << endl;
        return 0;
    }

    const long int roi_height = strtol(argv[6], &pEnd, 10);
    if (*pEnd || argv[6][0] == '\0')
    {
        cout << "<roi_height> should be an integer." << endl;
        return 0;
    }

    // check whether the input file exists
    ifstream fin(image_file_name, ios::binary);
    if (!fin)
    {
        cout << "The input file does not exist." << endl;
        return 0;
    }

    // get size of the file
    fin.seekg(0, fin.end);
    const long int size = fin.tellg();
    fin.seekg(0, fin.beg);
    // cout << size << endl;

    // check whether the content of the input file is empty
    if (size == 0)
    {
        cout << "The input file is empty." << endl;
        return 0;
    }

    // check the size of the file
    if (size % 4 != 0)
    {
        cout << "The format of the file is not correct." << endl;
        return 0;
    }

    const long int total_input_pixel = size / 4;

    // check whether the width of image is correct
    if (width_of_image <= 0)
    {
        cout << "The width of image should be positive integer." << endl;
        return 0;
    }

    if (total_input_pixel % width_of_image != 0)
    {
        cout << "The width of image should be a factor of the total number of pixel." << endl;
        return 0;
    }

    // get height_of_image
    const long int height_of_image = total_input_pixel / width_of_image;
    // cout << "original image resolution: " << width_of_image << " x " << height_of_image << endl;

    // check roi_startx
    if (roi_startx < 0 || roi_startx >= width_of_image)
    {
        cout << "The starting x-coordinate of the output ROI image is out of range." << endl;
        return 0;
    }

    // check roi_starty
    if (roi_starty < 0 || roi_starty >= height_of_image)
    {
        cout << "The starting y-coordinate of the output ROI image is out of range." << endl;
        return 0;
    }

    // check roi_width
    if (roi_width <= 0)
    {
        cout << "The width of the ROI image should be positive integer." << endl;
        return 0;
    }

    if (roi_startx + roi_width - 1 >= width_of_image)
    {
        cout << "The width of the ROI image is out of range." << endl;
        return 0;
    }

    // check roi_height
    if (roi_height <= 0)
    {
        cout << "The height of the ROI image should be positive integer." << endl;
        return 0;
    }

    if (roi_starty + roi_height - 1 >= height_of_image)
    {
        cout << "The height of the ROI image is out of range." << endl;
        return 0;
    }

    // variables for file IO
    const long int buffer_size = 1000;
    const long int quotient = roi_width / buffer_size;
    const long int remainder = roi_width % buffer_size;
    char buffer[buffer_size * 4];

    fin.seekg((roi_starty * width_of_image + roi_startx) * 4, fin.beg);
    long int y = 0;

    ofstream fout("out.dat", ios::binary);
    // ofstream fout_test("image.txt");
    // float pixel;
    // char *pixel_bytes = (char *)&pixel;
    while (true)
    {
        for (long int i = 0; i < quotient; i++)
        {
            // for fout
            fin.read(buffer, buffer_size * 4);
            fout.write(buffer, buffer_size * 4);

            // for fout_test
            // for (long int j = 0; j < buffer_size; j++)
            // {
            //     pixel_bytes[0] = buffer[4 * j];
            //     pixel_bytes[1] = buffer[4 * j + 1];
            //     pixel_bytes[2] = buffer[4 * j + 2];
            //     pixel_bytes[3] = buffer[4 * j + 3];

            //     if (remainder == 0 && i == quotient - 1 && j == buffer_size - 1)
            //     {
            //         fout_test << pixel << endl;
            //     }
            //     else
            //     {
            //         fout_test << pixel << ", ";
            //     }
            // }
        }

        // for remainder
        fin.read(buffer, remainder * 4);
        fout.write(buffer, remainder * 4);

        // for fout_test
        // for (long int j = 0; j < remainder; j++)
        // {
        //     pixel_bytes[0] = buffer[4 * j];
        //     pixel_bytes[1] = buffer[4 * j + 1];
        //     pixel_bytes[2] = buffer[4 * j + 2];
        //     pixel_bytes[3] = buffer[4 * j + 3];

        //     if (j == remainder - 1)
        //     {
        //         fout_test << pixel << endl;
        //     }
        //     else
        //     {
        //         fout_test << pixel << ", ";
        //     }
        // }

        y++;
        if (y == roi_height)
            break;

        fin.seekg((width_of_image - roi_width) * 4, fin.cur);
    }

    fin.close();
    fout.close();
    // fout_test.close();

    cout << "OK" << endl;
}