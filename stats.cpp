
#include "stats.h"

stats::stats(PNG & im){

/* your code here */

vector<vector<vector<int>>> temp(im.width(), vector<vector<int>>(im.height(), vector<int>(35)));

for(unsigned int i = 0; i < im.width(); i++){
  vector<double> hueX;
  vector<double> hueY;
  vector<double> sat;
  vector<double> lum;
  double hX = 0;
  double hY = 0;
  double s  = 0;
  double l  = 0;

  for(unsigned int j = 0; j < im.height(); j++){
    hX += cos(im.getPixel(i,j)->h * PI/180);
    hY += sin(im.getPixel(i,j)->h * PI/180);
    s  += im.getPixel(i,j)->s;
    l  += im.getPixel(i,j)->l;

    if(i == 0){
      hueX.push_back(hX);
      hueY.push_back(hY);
      sat.push_back(s);
      lum.push_back(l);
    }
    else{
      hueX.push_back(hX + sumHueX[i-1][j]);
      hueY.push_back(hY + sumHueY[i-1][j]);
      sat.push_back(s + sumSat[i-1][j]);
      lum.push_back(l + sumLum[i-1][j]);
    }

    unsigned int histK = (im.getPixel(i,j)->h) / 10;
    for(unsigned int k = 0; k < 36; k++){
      if(i == 0 && j == 0)
        if(k == histK)
          temp[0][0][k] = 1;
        else
          temp[0][0][k]=0;
      else if(i == 0){
        if(k == histK)
          temp[i][j][k] = temp[i][j-1][k] + 1;
        else
          temp[i][j][k] = temp[i][j-1][k];
      }
      else if(j == 0){
        if(k == histK)
          temp[i][j][k] = temp[i-1][j][k] + 1;
        else
          temp[i][j][k] = temp[i-1][j][k];
      }
      else{
        if(k == histK)
          temp[i][j][k] = temp[i-1][j][k] + temp[i][j-1][k] - temp[i-1][j-1][k] + 1;
        else
          temp[i][j][k] = temp[i-1][j][k] + temp[i][j-1][k] - temp[i-1][j-1][k];
      }
    }

  }
  sumHueX.push_back(hueX);
  sumHueY.push_back(hueY);
  sumSat.push_back(sat);
  sumLum.push_back(lum);
}
hist = temp;

}

long stats::rectArea(pair<int,int> ul, pair<int,int> lr){

/* your code here */
return (lr.first-ul.first+1)*(lr.second-ul.second+1);

}

HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){
  /* your code here */
long numPixel = rectArea(ul,lr);

if(ul.first==0 && ul.second==0){
  double avg_X = sumHueX[lr.first][lr.second]/numPixel;
  double avg_Y = sumHueY[lr.first][lr.second]/numPixel;
  double avg_h = atan2(avg_Y, avg_X) * 180 / PI;
  if(avg_h < 0 )
    avg_h += 360;
  double avg_s = sumSat[lr.first][lr.second]/numPixel;
  double avg_l = sumLum[lr.first][lr.second]/numPixel;
  return HSLAPixel(avg_h, avg_s, avg_l, 1);
}
else if(ul.first == 0){
  double avg_X = (sumHueX[lr.first][lr.second]-sumHueX[lr.first][ul.second-1])/numPixel;
  double avg_Y = (sumHueY[lr.first][lr.second]-sumHueY[lr.first][ul.second-1])/numPixel;
  double avg_h = atan2(avg_Y, avg_X) * 180 / PI;
  if(avg_h < 0 )
    avg_h += 360;
  double avg_s = (sumSat[lr.first][lr.second]-sumSat[lr.first][ul.second-1])/numPixel;
  double avg_l = (sumLum[lr.first][lr.second]-sumLum[lr.first][ul.second-1])/numPixel;
  return HSLAPixel(avg_h, avg_s, avg_l, 1);
}
else if(ul.second == 0){
  double avg_X = (sumHueX[lr.first][lr.second]-sumHueX[ul.first-1][lr.second])/numPixel;
  double avg_Y = (sumHueY[lr.first][lr.second]-sumHueY[ul.first-1][lr.second])/numPixel;
  double avg_h = atan2(avg_Y, avg_X) * 180 / PI;
  if(avg_h < 0 )
    avg_h += 360;
  double avg_s = (sumSat[lr.first][lr.second]-sumSat[ul.first-1][lr.second])/numPixel;
  double avg_l = (sumLum[lr.first][lr.second]-sumLum[ul.first-1][lr.second])/numPixel;
  return HSLAPixel(avg_h, avg_s, avg_l, 1);
}

double avg_X = (sumHueX[lr.first][lr.second]-sumHueX[lr.first][ul.second-1]
  -sumHueX[ul.first-1][lr.second]+sumHueX[ul.first-1][ul.second-1]) / numPixel;
double avg_Y = (sumHueY[lr.first][lr.second]-sumHueY[lr.first][ul.second-1]
  -sumHueY[ul.first-1][lr.second]+sumHueY[ul.first-1][ul.second-1]) / numPixel;
double avg_h = atan2(avg_Y, avg_X) * 180 / PI;
if(avg_h < 0 )
  avg_h += 360;

double avg_s = (sumSat[lr.first][lr.second]-sumSat[lr.first][ul.second-1]
    -sumSat[ul.first-1][lr.second]+sumSat[ul.first-1][ul.second-1]) / numPixel;
double avg_l = (sumLum[lr.first][lr.second]-sumLum[lr.first][ul.second-1]
    -sumLum[ul.first-1][lr.second]+sumLum[ul.first-1][ul.second-1]) / numPixel;

return HSLAPixel(avg_h, avg_s, avg_l, 1);

}

vector<int> stats::buildHist(pair<int,int> ul, pair<int,int> lr){

/* your code here */
vector<int> his;
for(int k = 0; k < 36; k++){
  if(ul.first == 0 && ul.second == 0)
    his.push_back(hist[lr.first][lr.second][k]);
  else if(ul.first == 0)
    his.push_back(hist[lr.first][lr.second][k]-hist[lr.first][ul.second-1][k]);
  else if(ul.second == 0)
    his.push_back(hist[lr.first][lr.second][k]-hist[ul.first-1][lr.second][k]);
  else
    his.push_back(hist[lr.first][lr.second][k]-hist[lr.first][ul.second-1][k]
      -hist[ul.first-1][lr.second][k]+hist[ul.first-1][ul.second-1][k]);
}
return his;

}

// takes a distribution and returns entropy
// partially implemented so as to avoid rounding issues.
double stats::entropy(vector<int> & distn,int area){

    double entropy = 0.;

/* your code here */

    for (int i = 0; i < 36; i++) {
        if (distn[i] > 0 )
            entropy += ((double) distn[i]/(double) area)
                                    * log2((double) distn[i]/(double) area);
    }

    return  -1 * entropy;

}

double stats::entropy(pair<int,int> ul, pair<int,int> lr){

/* your code here */
vector<int> his = buildHist(ul,lr);
return entropy(his,rectArea(ul,lr));

}
