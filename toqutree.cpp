
/**
 *
 * toqutree (pa3)
 * significant modification of a quadtree .
 * toqutree.cpp
 * This file will be used for grading.
 *
 */

#include "toqutree.h"

toqutree::Node::Node(pair<int,int> ctr, int dim, HSLAPixel a)
	:center(ctr),dimension(dim),avg(a),NW(NULL),NE(NULL),SE(NULL),SW(NULL)
	{}

toqutree::~toqutree(){
	clear(root);
}

toqutree::toqutree(const toqutree & other) {
	root = copy(other.root);
}


toqutree & toqutree::operator=(const toqutree & rhs){
	if (this != &rhs) {
		clear(root);
		root = copy(rhs.root);
	}
	return *this;
}

toqutree::toqutree(PNG & imIn, int k){

/* This constructor grabs the 2^k x 2^k sub-image centered */
/* in imIn and uses it to build a quadtree. It may assume  */
/* that imIn is large enough to contain an image of that size. */

/* your code here */
PNG* kkk = &imIn;
PNG* kSize = subImage(kkk,(imIn.width()-pow(2,k))/2,(imIn.height()-pow(2,k))/2,k);
root = buildTree(kSize,k);
delete kSize;
}

int toqutree::size() {
/* your code here */
int s = 0;
for(int i = 0; i < root->dimension+1; i++)
	s+=pow(4,i);
return s;
}


toqutree::Node * toqutree::buildTree(PNG * im, int k) {
/* your code here */
Node* temp;
if(k == 0){
	temp = new Node(pair<int,int>(0,0), 0, *(im->getPixel(0,0)));
	return temp;
}
else{
	stats* stat = new stats(*im);
	temp = new Node(getCenter(im,k), k, stat->getAvg(pair<int,int>(0,0),pair<int,int>(pow(2,k)-1,pow(2,k)-1)));
	delete stat;
	//cout<<temp->center.first<<","<<temp->center.second<<" ";

	unsigned int x = temp->center.first;
	unsigned int y = temp->center.second;
	unsigned int subWidth = pow(2,k-1);

		PNG* newSE = subImage(im,x,y,k-1);
		temp -> SE = buildTree(newSE,k-1);
		delete newSE;

		PNG* newNE = subImage(im,x,(y+subWidth),k-1);
		temp -> NE = buildTree(newNE,k-1);
		delete newNE;

		PNG* newSW = subImage(im,(x+subWidth),y,k-1);
		temp -> SW = buildTree(newSW,k-1);
		delete newSW;

		PNG* newNW = subImage(im,(x+subWidth),(y+subWidth),k-1);
		temp -> NW = buildTree(newNW,k-1);
		delete newNW;

	return temp;
}
// Note that you will want to practice careful memory use
// In this function. We pass the dynamically allocated image
// via pointer so that it may be released after it is used .
// similarly, at each level of the tree you will want to
// declare a dynamically allocated stats object, and free it
// once you've used it to choose a split point, and calculate
// an average.
}

PNG* toqutree::subImage(PNG* orig, int x, int y, int newK){
	int newWidth = pow(2,newK);
	PNG* newImage = new PNG(newWidth,newWidth);
	int origWidth = orig->width();
	for(unsigned int i = 0; i < newWidth; i++){
		for(unsigned int j = 0; j < newWidth; j++){
			 HSLAPixel* newPixel = newImage->getPixel(i,j);
			 HSLAPixel* oldPixel = orig->getPixel( (x+i)%origWidth,(y+j)%origWidth);
			 newPixel -> h = oldPixel -> h;
			 newPixel -> s = oldPixel -> s;
			 newPixel -> l = oldPixel -> l;
			 newPixel -> a = oldPixel -> a;
		}
	}
	return newImage;
}


PNG toqutree::render(){
// My algorithm for this problem included a helper function
// that was analogous to Find in a BST, but it navigated the
// quadtree, instead.

/* your code here */
	int w = pow(2,root->dimension);
	PNG* im = new PNG(w,w);
	// render(root,im,0,0,0,0);
	vector<vector<HSLAPixel*>> pixels;
	for(int i = 0; i<w; i++){
		vector<HSLAPixel*> p;
		for(int j = 0; j<w; j++){
			p.push_back(im->getPixel(i,j));
		}
		pixels.push_back(p);
	}
	getColor(root,pixels);
	return *im;
}

void toqutree::getColor(Node* croot, vector<vector<HSLAPixel*>> pixels){
	int thisWidth = pow(2,croot->dimension);
	int nextWidth = thisWidth/2;
	if(croot->SE!=NULL){
		int xSeNe = croot->center.first;
		int xNwSw = (xSeNe+nextWidth)%thisWidth;
		int ySeSw = croot->center.second;
		int yNeNw = (ySeSw+nextWidth)%thisWidth;
		vector<vector<HSLAPixel*>> se;
		for(int i = 0; i < nextWidth; i++){
			vector<HSLAPixel*> sese;
			for(int j = 0; j < nextWidth; j++){
				sese.push_back(pixels[(xSeNe+i)%thisWidth][(ySeSw+j)%thisWidth]);
			}
			se.push_back(sese);
		}
		getColor(croot->SE,se);
		vector<vector<HSLAPixel*>> ne;
		for(int i = 0; i < nextWidth; i++){
			vector<HSLAPixel*> nene;
			for(int j = 0; j < nextWidth; j++){
				nene.push_back(pixels[(xSeNe+i)%thisWidth][(yNeNw+j)%thisWidth]);
			}
			ne.push_back(nene);
		}
		getColor(croot->NE,ne);
		vector<vector<HSLAPixel*>> sw;
		for(int i = 0; i < nextWidth; i++){
			vector<HSLAPixel*> swsw;
			for(int j = 0; j < nextWidth; j++){
				swsw.push_back(pixels[(xNwSw+i)%thisWidth][(ySeSw+j)%thisWidth]);
			}
			sw.push_back(swsw);
		}
		getColor(croot->SW,sw);
		vector<vector<HSLAPixel*>> nw;
		for(int i = 0; i < nextWidth; i++){
			vector<HSLAPixel*> nwnw;
			for(int j = 0; j < nextWidth; j++){
				nwnw.push_back(pixels[(xNwSw+i)%thisWidth][(yNeNw+j)%thisWidth]);
			}
			nw.push_back(nwnw);
		}
		getColor(croot->NW,nw);

	}
	else{
		for(unsigned int i = 0; i < thisWidth; i++){
			for(unsigned int j = 0; j < thisWidth; j++){
				 HSLAPixel* newPixel = pixels[i][j];
				 HSLAPixel oldPixel = croot->avg;
				 newPixel -> h = oldPixel.h;
				 newPixel -> s = oldPixel.s;
				 newPixel -> l = oldPixel.l;
				 newPixel -> a = oldPixel.a;
			}
		}
	}
}

/* oops, i left the implementation of this one in the file! */
void toqutree::prune(double tol){
	prune(root,tol);
}
void toqutree::prune(Node* root, double tol){
	if(root != NULL){
		if((root->NW) != NULL){
			if(isWithinTol(root,tol,root->avg)){
				clear(root->NW);
				clear(root->NE);
				clear(root->SW);
				clear(root->SE);
			}
			else{
				prune(root->NW,tol);
				prune(root->NE,tol);
				prune(root->SW,tol);
				prune(root->SE,tol);
			}
		}
	}
}
bool toqutree::isWithinTol(Node* root, double tol, HSLAPixel avgP){
	bool isTol = true;
	if(root != NULL){
		if((root->NW) != NULL){
			isTol = isTol&&isWithinTol(root->NW,tol,avgP);
			if(!isTol)
				return isTol;
			isTol = isTol&&isWithinTol(root->NE,tol,avgP);
			if(!isTol)
				return isTol;
			isTol = isTol&&isWithinTol(root->SW,tol,avgP);
			if(!isTol)
				return isTol;
			isTol = isTol&&isWithinTol(root->SE,tol,avgP);
			if(!isTol)
				return isTol;

			return true;
		}
		return isTol && root->avg.dist(avgP)<=tol;
	}
	return false;
}

/* called by destructor and assignment operator*/
void toqutree::clear(Node * & curr){
/* your code here */
if(curr != NULL){
	if((curr->NW) != NULL){
		clear(curr->NW);
		curr->NW = NULL;
		clear(curr->NE);
		curr->NE = NULL;
		clear(curr->SE);
		curr->SE = NULL;
		clear(curr->SW);
		curr->SW = NULL;
	}
	delete curr;
	curr = NULL;
}
}

/* done */
/* called by assignment operator and copy constructor */
toqutree::Node * toqutree::copy(const Node * other) {

/* your code here */
	Node* temp = new Node(other->center,other->dimension,other->avg);
	if((other->NW) != NULL){
		temp->NW = copy(other->NW);
		temp->NE = copy(other->NE);
		temp->SE = copy(other->SE);
		temp->SW = copy(other->SW);
	}
	return temp;
}

double toqutree::avgEntropy(PNG* imIn, pair<int,int> se, stats stat){
	double allEntropy;
	int thisWidth = imIn->width();
	int nextWidth = thisWidth/2;
	int xSeNe = se.first;
	int xNwSw = (se.first+nextWidth)%thisWidth;
	int ySeSw = se.second;
	int yNeNw = (se.second+nextWidth)%thisWidth;

	if(se.first<nextWidth && se.second<nextWidth){
		vector<int> nw1 = stat.buildHist(pair<int,int>(xNwSw,yNeNw),pair<int,int>(thisWidth-1,thisWidth-1));
		vector<int> nw2 = stat.buildHist(pair<int,int>(0,0),pair<int,int>(xSeNe-1,ySeSw-1));
		vector<int> nw3 = stat.buildHist(pair<int,int>(xNwSw,0),pair<int,int>(thisWidth-1,ySeSw-1));
		vector<int> nw4 = stat.buildHist(pair<int,int>(0,yNeNw),pair<int,int>(xSeNe-1,thisWidth-1));
		vector<int> nw;
		for(int i = 0; i < 36; i++)
			nw.push_back(nw1[i]+nw2[i]+nw3[i]+nw4[i]);
		double nwE = stat.entropy(nw,nextWidth*nextWidth);
		allEntropy = nwE;

		vector<int> sw1 = stat.buildHist(pair<int,int>(xNwSw,ySeSw),pair<int,int>(thisWidth-1,yNeNw-1));
		vector<int> sw2 = stat.buildHist(pair<int,int>(0,ySeSw),pair<int,int>(xSeNe-1,yNeNw-1));
		vector<int> sw;
		for(int i = 0; i < 36; i++)
			sw.push_back(sw1[i]+sw2[i]);
		double swE = stat.entropy(sw,nextWidth*nextWidth);
		allEntropy += swE;

		vector<int> ne1 = stat.buildHist(pair<int,int>(xSeNe,yNeNw),pair<int,int>(xNwSw-1,thisWidth-1));
		vector<int> ne2 = stat.buildHist(pair<int,int>(xSeNe,0),pair<int,int>(xNwSw-1,ySeSw-1));
		vector<int> ne;
		for(int i = 0; i < 36; i++)
			ne.push_back(ne1[i]+ne2[i]);
		double neE = stat.entropy(ne,nextWidth*nextWidth);
		allEntropy += neE;

		vector<int> se = stat.buildHist(pair<int,int>(xSeNe,ySeSw),pair<int,int>(xNwSw-1,yNeNw-1));
		double seE = stat.entropy(se,nextWidth*nextWidth);
		allEntropy += seE;
	}
	else if(se.first<nextWidth){
		if(se.second == nextWidth){
			vector<int> sw1 = stat.buildHist(pair<int,int>(xNwSw,ySeSw),pair<int,int>(thisWidth-1,thisWidth-1));
			vector<int> sw2 = stat.buildHist(pair<int,int>(0,ySeSw),pair<int,int>(xSeNe-1,thisWidth-1));
			vector<int> sw;
			for(int i = 0; i < 36; i++)
				sw.push_back(sw1[i]+sw2[i]);
			double swE = stat.entropy(sw,nextWidth*nextWidth);
			allEntropy = swE;

			vector<int> nw1 = stat.buildHist(pair<int,int>(xNwSw,yNeNw),pair<int,int>(thisWidth-1,ySeSw-1));
			vector<int> nw2 = stat.buildHist(pair<int,int>(0,0),pair<int,int>(xSeNe-1,ySeSw-1));
			vector<int> nw;
			for(int i = 0; i < 36; i++)
				nw.push_back(nw1[i]+nw2[i]);
			double nwE = stat.entropy(nw,nextWidth*nextWidth);
			allEntropy += nwE;

			vector<int> se = stat.buildHist(pair<int,int>(xSeNe,ySeSw),pair<int,int>(xNwSw-1,thisWidth-1));
			double seE = stat.entropy(se,nextWidth*nextWidth);
			allEntropy += seE;

			vector<int> ne = stat.buildHist(pair<int,int>(xSeNe,yNeNw),pair<int,int>(xNwSw-1,ySeSw-1));
			double neE = stat.entropy(ne,nextWidth*nextWidth);
			allEntropy += neE;
		}
		else{
			vector<int> sw1 = stat.buildHist(pair<int,int>(xNwSw,ySeSw),pair<int,int>(thisWidth-1,thisWidth-1));
			vector<int> sw2 = stat.buildHist(pair<int,int>(0,0),pair<int,int>(xSeNe-1,yNeNw-1));
			vector<int> sw3 = stat.buildHist(pair<int,int>(xNwSw,0),pair<int,int>(thisWidth-1,yNeNw-1));
			vector<int> sw4 = stat.buildHist(pair<int,int>(0,ySeSw),pair<int,int>(xSeNe-1,thisWidth-1));
			vector<int> sw;
			for(int i = 0; i < 36; i++)
				sw.push_back(sw1[i]+sw2[i]+sw3[i]+sw4[i]);
			double swE = stat.entropy(sw,nextWidth*nextWidth);
			allEntropy = swE;

			vector<int> se1 = stat.buildHist(pair<int,int>(xSeNe,ySeSw),pair<int,int>(xNwSw-1,thisWidth-1));
			vector<int> se2 = stat.buildHist(pair<int,int>(xSeNe,0),pair<int,int>(xNwSw-1,yNeNw-1));
			vector<int> se;
			for(int i = 0; i < 36; i++)
				se.push_back(se1[i]+se2[i]);
			double seE = stat.entropy(se,nextWidth*nextWidth);
			allEntropy += seE;

			vector<int> nw1 = stat.buildHist(pair<int,int>(xNwSw,yNeNw),pair<int,int>(thisWidth-1,ySeSw-1));
			vector<int> nw2 = stat.buildHist(pair<int,int>(0,yNeNw),pair<int,int>(xSeNe-1,ySeSw-1));
			vector<int> nw;
			for(int i = 0; i < 36; i++)
				nw.push_back(nw1[i]+nw2[i]);
			double nwE = stat.entropy(nw,nextWidth*nextWidth);
			allEntropy += nwE;

			vector<int> ne = stat.buildHist(pair<int,int>(xSeNe,yNeNw),pair<int,int>(xNwSw-1,ySeSw-1));
			double neE = stat.entropy(ne,nextWidth*nextWidth);
			allEntropy += neE;
		}
	}
	else if(se.second<nextWidth){
		if(se.first == nextWidth){
			vector<int> ne1 = stat.buildHist(pair<int,int>(xSeNe,yNeNw),pair<int,int>(thisWidth-1,thisWidth-1));
			vector<int> ne2 = stat.buildHist(pair<int,int>(xSeNe,0),pair<int,int>(thisWidth-1,ySeSw-1));
			vector<int> ne;
			for(int i = 0; i < 36; i++)
				ne.push_back(ne1[i]+ne2[i]);
			double neE = stat.entropy(ne,nextWidth*nextWidth);
			allEntropy = neE;

			vector<int> nw1 = stat.buildHist(pair<int,int>(xNwSw,yNeNw),pair<int,int>(xSeNe-1,thisWidth-1));
			vector<int> nw2 = stat.buildHist(pair<int,int>(0,0),pair<int,int>(xSeNe-1,ySeSw-1));
			vector<int> nw;
			for(int i = 0; i < 36; i++)
				nw.push_back(nw1[i]+nw2[i]);
			double nwE = stat.entropy(nw,nextWidth*nextWidth);
			allEntropy += nwE;

			vector<int> se = stat.buildHist(pair<int,int>(xSeNe,ySeSw),pair<int,int>(thisWidth-1,yNeNw-1));
			double seE = stat.entropy(se,nextWidth*nextWidth);
			allEntropy += seE;

			vector<int> sw = stat.buildHist(pair<int,int>(xNwSw,ySeSw),pair<int,int>(xSeNe-1,yNeNw-1));
			double swE = stat.entropy(sw,nextWidth*nextWidth);
			allEntropy += swE;
		}
		else{
			vector<int> ne1 = stat.buildHist(pair<int,int>(xSeNe,yNeNw),pair<int,int>(thisWidth-1,thisWidth-1));
			vector<int> ne2 = stat.buildHist(pair<int,int>(0,0),pair<int,int>(xNwSw-1,ySeSw-1));
			vector<int> ne3 = stat.buildHist(pair<int,int>(xSeNe,0),pair<int,int>(thisWidth-1,ySeSw-1));
			vector<int> ne4 = stat.buildHist(pair<int,int>(0,yNeNw),pair<int,int>(xNwSw-1,thisWidth-1));
			vector<int> ne;
			for(int i = 0; i < 36; i++)
				ne.push_back(ne1[i]+ne2[i]+ne3[i]+ne4[i]);
			double neE = stat.entropy(ne,nextWidth*nextWidth);
			allEntropy = neE;

			vector<int> se1 = stat.buildHist(pair<int,int>(xSeNe,ySeSw),pair<int,int>(thisWidth-1,yNeNw-1));
			vector<int> se2 = stat.buildHist(pair<int,int>(0,ySeSw),pair<int,int>(xNwSw-1,yNeNw-1));
			vector<int> se;
			for(int i = 0; i < 36; i++)
				se.push_back(se1[i]+se2[i]);
			double seE = stat.entropy(se,nextWidth*nextWidth);
			allEntropy += seE;

			vector<int> nw1 = stat.buildHist(pair<int,int>(xNwSw,yNeNw),pair<int,int>(xSeNe-1,thisWidth-1));
			vector<int> nw2 = stat.buildHist(pair<int,int>(xNwSw,0),pair<int,int>(xSeNe-1,ySeSw-1));
			vector<int> nw;
			for(int i = 0; i < 36; i++)
				nw.push_back(nw1[i]+nw2[i]);
			double nwE = stat.entropy(nw,nextWidth*nextWidth);
			allEntropy += nwE;

			vector<int> sw = stat.buildHist(pair<int,int>(xNwSw,ySeSw),pair<int,int>(xSeNe-1,yNeNw-1));
			double swE = stat.entropy(sw,nextWidth*nextWidth);
			allEntropy += swE;
		}
	}
	else if(se.first == nextWidth && se.second == nextWidth){
		vector<int> sw = stat.buildHist(pair<int,int>(xNwSw,ySeSw),pair<int,int>(xSeNe-1,thisWidth-1));
		double swE = stat.entropy(sw,nextWidth*nextWidth);
		allEntropy = swE;

		vector<int> se = stat.buildHist(pair<int,int>(xSeNe,ySeSw),pair<int,int>(thisWidth-1,thisWidth-1));
		double seE = stat.entropy(se,nextWidth*nextWidth);
		allEntropy += seE;

		vector<int> nw = stat.buildHist(pair<int,int>(xNwSw,yNeNw),pair<int,int>(xSeNe-1,ySeSw-1));
		double nwE = stat.entropy(nw,nextWidth*nextWidth);
		allEntropy += nwE;

		vector<int> ne = stat.buildHist(pair<int,int>(xSeNe,yNeNw),pair<int,int>(thisWidth-1,ySeSw-1));
		double neE = stat.entropy(ne,nextWidth*nextWidth);
		allEntropy += neE;
	}
	else if(se.first == nextWidth){
		vector<int> sw1 = stat.buildHist(pair<int,int>(xNwSw,ySeSw),pair<int,int>(xSeNe-1,thisWidth-1));
		vector<int> sw2 = stat.buildHist(pair<int,int>(0,0),pair<int,int>(xSeNe-1,yNeNw-1));
		vector<int> sw;
		for(int i = 0; i < 36; i++)
			sw.push_back(sw1[i]+sw2[i]);
		double swE = stat.entropy(sw,nextWidth*nextWidth);
		allEntropy = swE;

		vector<int> se1 = stat.buildHist(pair<int,int>(xSeNe,ySeSw),pair<int,int>(thisWidth-1,thisWidth-1));
		vector<int> se2 = stat.buildHist(pair<int,int>(xSeNe,0),pair<int,int>(thisWidth-1,yNeNw-1));
		vector<int> se;
		for(int i = 0; i < 36; i++)
			se.push_back(se1[i]+se2[i]);
		double seE = stat.entropy(se,nextWidth*nextWidth);
		allEntropy += seE;

		vector<int> ne = stat.buildHist(pair<int,int>(xSeNe,yNeNw),pair<int,int>(thisWidth-1,ySeSw-1));
		double neE = stat.entropy(ne,nextWidth*nextWidth);
		allEntropy += neE;

		vector<int> nw = stat.buildHist(pair<int,int>(xNwSw,yNeNw),pair<int,int>(xSeNe-1,ySeSw-1));
		double nwE = stat.entropy(nw,nextWidth*nextWidth);
		allEntropy += nwE;
	}
	else if(se.second == nextWidth){
		vector<int> ne1 = stat.buildHist(pair<int,int>(xSeNe,yNeNw),pair<int,int>(thisWidth-1,ySeSw-1));
		vector<int> ne2 = stat.buildHist(pair<int,int>(0,0),pair<int,int>(xNwSw-1,ySeSw-1));
		vector<int> ne;
		for(int i = 0; i < 36; i++)
			ne.push_back(ne1[i]+ne2[i]);
		double neE = stat.entropy(ne,nextWidth*nextWidth);
		allEntropy = neE;

		vector<int> se1 = stat.buildHist(pair<int,int>(xSeNe,ySeSw),pair<int,int>(thisWidth-1,thisWidth-1));
		vector<int> se2 = stat.buildHist(pair<int,int>(0,ySeSw),pair<int,int>(xNwSw-1,thisWidth-1));
		vector<int> se;
		for(int i = 0; i < 36; i++)
			se.push_back(se1[i]+se2[i]);
		double seE = stat.entropy(se,nextWidth*nextWidth);
		allEntropy += seE;

		vector<int> sw = stat.buildHist(pair<int,int>(xNwSw,ySeSw),pair<int,int>(xSeNe-1,thisWidth-1));
		double swE = stat.entropy(sw,nextWidth*nextWidth);
		allEntropy += swE;

		vector<int> nw = stat.buildHist(pair<int,int>(xNwSw,yNeNw),pair<int,int>(xSeNe-1,ySeSw-1));
		double nwE = stat.entropy(nw,nextWidth*nextWidth);
		allEntropy += nwE;
	}
	else{
		vector<int> se1 = stat.buildHist(pair<int,int>(xSeNe,ySeSw),pair<int,int>(thisWidth-1,thisWidth-1));
		vector<int> se2 = stat.buildHist(pair<int,int>(0,0),pair<int,int>(xNwSw-1,yNeNw-1));
		vector<int> se3 = stat.buildHist(pair<int,int>(xSeNe,0),pair<int,int>(thisWidth-1,yNeNw-1));
		vector<int> se4 = stat.buildHist(pair<int,int>(0,ySeSw),pair<int,int>(xNwSw-1,thisWidth-1));
		vector<int> se;
		for(int i = 0; i < 36; i++)
			se.push_back(se1[i]+se2[i]+se3[i]+se4[i]);
		double seE = stat.entropy(se,nextWidth*nextWidth);
		allEntropy = seE;

		vector<int> ne1 = stat.buildHist(pair<int,int>(xSeNe,yNeNw),pair<int,int>(thisWidth-1,ySeSw-1));
		vector<int> ne2 = stat.buildHist(pair<int,int>(0,yNeNw),pair<int,int>(xNwSw-1,ySeSw-1));
		vector<int> ne;
		for(int i = 0; i < 36; i++)
			ne.push_back(ne1[i]+ne2[i]);
		double neE = stat.entropy(ne,nextWidth*nextWidth);
		allEntropy += neE;

		vector<int> sw1 = stat.buildHist(pair<int,int>(xNwSw,ySeSw),pair<int,int>(xSeNe-1,thisWidth-1));
		vector<int> sw2 = stat.buildHist(pair<int,int>(xNwSw,0),pair<int,int>(xSeNe-1,yNeNw-1));
		vector<int> sw;
		for(int i = 0; i < 36; i++)
			sw.push_back(sw1[i]+sw2[i]);
		double swE = stat.entropy(sw,nextWidth*nextWidth);
		allEntropy += swE;

		vector<int> nw = stat.buildHist(pair<int,int>(xNwSw,yNeNw),pair<int,int>(xSeNe-1,ySeSw-1));
		double nwE = stat.entropy(nw,nextWidth*nextWidth);
		allEntropy += nwE;
	}

	return allEntropy;
}

pair<int,int> toqutree::getCenter(PNG* imIn, int k){
	pair <int,int> ctr (0,0);
	stats stat(*imIn);

	if(k == 1){
		ctr.first = 1;
		ctr.second = 1;
	}
	else if(k > 1){
		ctr.first = pow(2,k-2);
		ctr.second = pow(2,k-2);
		double min = avgEntropy(imIn,ctr,stat);
		for(int i = pow(2,k-2); i < 3*pow(2,k-2); i++){
			for(int j = pow(2,k-2); j < 3*pow(2,k-2); j++){
					double entropy = avgEntropy(imIn,pair<int,int>(i,j),stat);
					if(entropy<min){
						min = entropy;
						ctr.first = i;
						ctr.second = j;
					}
			}
		}
	}
	return ctr;
}
