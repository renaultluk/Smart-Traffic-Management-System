struct lineSensor {
    int index;
    float blank_lower;
    float blank_upper;
    float line_lower;
    float line_upper;
    float led_lower;
    float led_upper;
};

lineSensor line_sensors[8] = {
    {0, 11.0, 16.0, 1.0, 3.0, 4.0, 8.0},
    {1, 28.0, 33.0, 1.0, 8.0, 0.0, 0.0},
    {2, 90.0, 117.0, 82.0, 90.0, 73.0, 80.0},
    {3, 33.0, 38.0, 1.0, 4.0, 9.0, 14.0},
    {4, 3.0, 10.0, 1.0, 2.0, 0.0, 0.0},
    {5, 2.0, 9.0, 1.0, 1.0, 0.0, 0.0},
    {6, 12.0, 20.0, 1.0, 1.0, 4.0, 8.0},
    {7, 4.0, 13.0, 1.0, 1.0, 2.0, 3.0}
};