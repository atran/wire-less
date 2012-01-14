Instead of generating static lines, create a LinesGenerator class that produces lines at the blob's outer square boundaries.

LineGenerator {
    x1 = { horizontal boundaries of blob };
    
    y_init;
    y_acceleration;
    y_thickness;
    
    void::generate() {
        stack lines (accumulating position);
        set colors;
    }
    void::update() {
        y_acceleration++;
    }
}