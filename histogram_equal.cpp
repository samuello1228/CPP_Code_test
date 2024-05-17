#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <vector>
using namespace std;

void counting(
    char *buffer, long int buffer_size,
    float &pixel, char *pixel_bytes,
    float min_lot_value, float max_lot_value,
    float input_bin_width,
    long int &lower, long int &higher,
    vector<long int> &hist_input)
{
    for (long int j = 0; j < buffer_size; j++)
    {
        pixel_bytes[0] = buffer[4 * j];
        pixel_bytes[1] = buffer[4 * j + 1];
        pixel_bytes[2] = buffer[4 * j + 2];
        pixel_bytes[3] = buffer[4 * j + 3];
        // cout << pixel << " ";

        // counting
        if (pixel < min_lot_value)
        {
            lower++;
        }
        else if (pixel >= max_lot_value)
        {
            higher++;
        }
        else
        {
            const int input_index = floor((pixel - min_lot_value) / input_bin_width);
            hist_input[input_index]++;
        }
    }
}

void set_new_pixel(
    char *buffer, long int buffer_size,
    float &pixel, char *pixel_bytes,
    float min_lot_value, float max_lot_value,
    float lowest_value, float highest_value,
    float range, float input_bin_width, float output_bin_width,
    vector<float> &hist_cumulative,
    // long int &row, long int &column, long int width_of_image, ofstream &fout_test,
    vector<long int> &hist_output)
{
    for (long int j = 0; j < buffer_size; j++)
    {
        pixel_bytes[0] = buffer[4 * j];
        pixel_bytes[1] = buffer[4 * j + 1];
        pixel_bytes[2] = buffer[4 * j + 2];
        pixel_bytes[3] = buffer[4 * j + 3];
        // cout << pixel << " ";

        // set new pixel
        if (pixel < min_lot_value)
        {
            pixel = lowest_value;
        }
        else if (pixel >= max_lot_value)
        {
            pixel = highest_value;
        }
        else
        {
            const int input_index = floor((pixel - min_lot_value) / input_bin_width);
            pixel = min_lot_value + hist_cumulative[input_index] * range;

            const int output_index = ceil((pixel - min_lot_value) / output_bin_width) - 1;
            if (output_index >= 0)
                hist_output[output_index]++;
        }

        // write
        buffer[4 * j] = pixel_bytes[0];
        buffer[4 * j + 1] = pixel_bytes[1];
        buffer[4 * j + 2] = pixel_bytes[2];
        buffer[4 * j + 3] = pixel_bytes[3];

        // for fout_test
        // if (column == width_of_image - 1)
        // {
        //     fout_test << pixel << endl;
        //     row++;
        //     column = 0;
        // }
        // else
        // {
        //     fout_test << pixel << ", ";
        //     column++;
        // }
    }
}

int main(int argc, char *argv[])
{
    // check the number of argument
    if (argc != 8)
    {
        cout << "The number of argument should be 7." << endl;
        cout << "histogram_equal.exe <image file name> <min_lot_value> <max_lot_value> <input_lot> <output_lot> <lowest_value> <highest_value>" << endl;
        return 0;
    }

    // check all parameters
    const string image_file_name = argv[1];

    char *pEnd;
    const float min_lot_value = strtod(argv[2], &pEnd);
    if (*pEnd || argv[2][0] == '\0')
    {
        cout << "<min_lot_value> should be a floating point number." << endl;
        return 0;
    }

    const float max_lot_value = strtod(argv[3], &pEnd);
    if (*pEnd || argv[3][0] == '\0')
    {
        cout << "<max_lot_value> should be a floating point number." << endl;
        return 0;
    }

    const int input_lot = strtol(argv[4], &pEnd, 10);
    if (*pEnd || argv[4][0] == '\0')
    {
        cout << "<input_lot> should be an integer." << endl;
        return 0;
    }

    const int output_lot = strtol(argv[5], &pEnd, 10);
    if (*pEnd || argv[5][0] == '\0')
    {
        cout << "<output_lot> should be an integer." << endl;
        return 0;
    }

    const float lowest_value = strtod(argv[6], &pEnd);
    if (*pEnd || argv[6][0] == '\0')
    {
        cout << "<lowest_value> should be a floating point number." << endl;
        return 0;
    }

    const float highest_value = strtod(argv[7], &pEnd);
    if (*pEnd || argv[7][0] == '\0')
    {
        cout << "<highest_value> should be a floating point number." << endl;
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

    // check min_lot_value and max_lot_value
    if (min_lot_value >= max_lot_value)
    {
        cout << "<min_lot_value> should be less than <max_lot_value>" << endl;
        return 0;
    }

    // check whether input_lot is positive integer.
    if (input_lot <= 0)
    {
        cout << "<input_lot> should be positive integer." << endl;
        return 0;
    }

    // check whether output_lot is positive integer.
    if (output_lot <= 0)
    {
        cout << "<output_lot> should be positive integer." << endl;
        return 0;
    }

    // check lowest_value
    if (lowest_value >= min_lot_value)
    {
        cout << "<lowest_value> should be less than <min_lot_value>." << endl;
        return 0;
    }

    // check highest_value
    if (highest_value <= max_lot_value)
    {
        cout << "<highest_value> should be greater than <max_lot_value>." << endl;
        return 0;
    }

    // variables for file IO
    const long int buffer_size = 10000;
    const long int quotient = total_input_pixel / buffer_size;
    const long int remainder = total_input_pixel % buffer_size;
    char buffer[buffer_size * 4];

    float pixel;
    char *pixel_bytes = (char *)&pixel;

    // define some constant for histograms
    const float range = max_lot_value - min_lot_value;
    const float input_bin_width = range / input_lot;
    const float output_bin_width = range / output_lot;

    // initialize hist_input
    vector<long int> hist_input(input_lot, 0);

    long int lower = 0;
    long int higher = 0;

    // for quotient
    for (long int i = 0; i < quotient; i++)
    {
        fin.read(buffer, buffer_size * 4);
        counting(
            buffer, buffer_size,
            pixel, pixel_bytes,
            min_lot_value, max_lot_value,
            input_bin_width,
            lower, higher, hist_input);
    }

    // for remainder
    fin.read(buffer, remainder * 4);
    counting(
        buffer, remainder,
        pixel, pixel_bytes,
        min_lot_value, max_lot_value,
        input_bin_width,
        lower, higher, hist_input);

    // calculate hist_cumulative
    vector<float> hist_cumulative(input_lot, 0);
    hist_cumulative[0] = hist_input[0];
    for (int i = 1; i < input_lot; i++)
    {
        hist_cumulative[i] = hist_cumulative[i - 1] + hist_input[i];
    }

    for (int i = 0; i < input_lot; i++)
    {
        hist_cumulative[i] /= hist_cumulative[input_lot - 1];
    }

    // print hist_cumulative
    // cout << "hist_cumulative" << endl;
    // for (int i = 0; i < input_lot; i++)
    // {
    //     cout << i << ": " << hist_cumulative[i] << endl;
    // }

    // initialize hist_output
    vector<long int> hist_output(output_lot, 0);

    // reset file input
    fin.seekg(0, fin.beg);

    ofstream fout("outhisto.dat", ios::binary);

    // for fout_test
    // long int row = 0;
    // long int column = 0;
    // const long int width_of_image = 2400;
    // ofstream fout_test("image_equal.txt");

    // for quotient
    for (long int i = 0; i < quotient; i++)
    {
        fin.read(buffer, buffer_size * 4);
        set_new_pixel(
            buffer, buffer_size,
            pixel, pixel_bytes,
            min_lot_value, max_lot_value,
            lowest_value, highest_value,
            range, input_bin_width, output_bin_width,
            hist_cumulative,
            // row, column, width_of_image, fout_test,
            hist_output);
        fout.write(buffer, buffer_size * 4);
    }

    // for remainder
    fin.read(buffer, remainder * 4);
    set_new_pixel(
        buffer, remainder,
        pixel, pixel_bytes,
        min_lot_value, max_lot_value,
        lowest_value, highest_value,
        range, input_bin_width, output_bin_width,
        hist_cumulative,
        // row, column, width_of_image, fout_test,
        hist_output);
    fout.write(buffer, remainder * 4);

    fin.close();
    fout.close();
    // fout_test.close();

    // output information to outhisto.txt
    ofstream fout_txt("outhisto.txt");
    fout_txt << "Total input pixel: " << total_input_pixel << endl;
    fout_txt << "lower: " << lower << endl;
    fout_txt << "higher: " << higher << endl;

    // write Input lot
    fout_txt << "Input lot:" << endl;
    for (int i = 0; i < input_lot; i++)
    {
        fout_txt << min_lot_value + input_bin_width * i << ", " << hist_input[i] << endl;
    }

    // write Output lot
    fout_txt << "Output lot:" << endl;
    for (int i = 0; i < output_lot; i++)
    {
        fout_txt << min_lot_value + output_bin_width * i << ", " << hist_output[i] << endl;
    }

    fout_txt.close();
    cout << "OK" << endl;
}