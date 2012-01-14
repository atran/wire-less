ArrayList linez;

void setup() 
{
  size(400, 400);
  frameRate(30);
  linez = new ArrayList();
  for (int i=0;i<21;i++) {
    linez.add(new HLine(20*i, 10.0, 20));
  }
}

void draw() { 
  background(255);
  for (int i = linez.size()-1; i >= 0; i--) { 
    HLine lined = (HLine) linez.get(i);
    lined.update();
  }
} 
