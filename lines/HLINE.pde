class HLine { 
  float ypos, speed; 
  float weight;
  color col;
  HLine (float y, float s, float w, float c) { 
    ypos = y; 
    speed = s; 
    weight = w;
    col = color(c);
  }
  HLine (float y, float s, float w) {  
    ypos = y; 
    speed = s; 
    weight = w;
    col = color(random(0, 255));
  } 
  void update() { 
    ypos += speed; 
    if (ypos > height) { 
      ypos = 0; 
    } 
    fill(col);
    stroke(col);
    strokeWeight(weight);
    line(0, ypos, width, ypos); 
  } 
}
