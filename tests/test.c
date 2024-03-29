int dist(int d_x1, int d_y1, int d_x2, int d_y2) {
  return (d_x2 - d_x1) * (d_x2 - d_x1) + (d_y2 - d_y1) * (d_y2 - d_y1);
}

int check(int p1_x, int p1_y,
          int p2_x, int p2_y,
          int p3_x, int p3_y,
          int p4_x, int p4_y) {

  return dist(p1_x, p1_y, p2_x, p2_y) > 0
      && dist(p1_x, p1_y, p2_x, p2_y) == dist(p2_x, p2_y, p3_x, p3_y)
      && dist(p2_x, p2_y, p3_x, p3_y) == dist(p3_x, p3_y, p4_x, p4_y)
      && dist(p3_x, p3_y, p4_x, p4_y) == dist(p4_x, p4_y, p1_x, p1_y)
      && dist(p1_x, p1_y, p3_x, p3_y) == dist(p2_x, p2_y, p4_x, p4_y);
}
/*  */

int main() {
  int p1[2];
  int p2[2];
  int p3[2];
  int p4[2];
  int chk_res1, chk_res2, chk_res3;

  p1[0] = read();
  p1[1] = read();
  p2[0] = read();
  p2[1] = read();
  p3[0] = read();
  p3[1] = read();
  p4[0] = read();
  p4[1] = read();

  chk_res1 = check(p1[0], p1[1], p2[0], p2[1], p3[0], p3[1], p4[0], p4[1]);
  chk_res2 = check(p1[0], p1[1], p3[0], p3[1], p2[0], p2[1], p4[0], p4[1]);
  chk_res3 = check(p1[0], p1[1], p2[0], p2[1], p4[0], p4[1], p3[0], p3[1]);  
  write(chk_res1 || chk_res2 || chk_res3);
  return 0;
}
