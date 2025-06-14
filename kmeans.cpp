
#include "ocv_utils.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <chrono>

using namespace cv;
using namespace std;

void printHelp(const string& progName)
{
    cout << "Usage:\n\t " << progName << " <image_file> <K_num_of_clusters> [<image_ground_truth>]" << endl;
}

int main(int argc, char** argv)
{
    if (argc != 3 && argc != 4)
    {
        cout << " Incorrect number of arguments." << endl;
        printHelp(string(argv[0]));
        return EXIT_FAILURE;
    }

    const auto imageFilename = string(argv[1]);
    const string groundTruthFilename = (argc == 4) ? string(argv[3]) : string();
    const int k = stoi(argv[2]);

    // just for debugging
    {
        cout << " Program called with the following arguments:" << endl;
        cout << " image file: " << imageFilename << endl;
        cout << " k: " << k << endl;
        if(!groundTruthFilename.empty()) cout << " \tground truth segmentation: " << groundTruthFilename << endl;
    }

    // load the color image to process from file

    Mat m;
    m =  imread(argv[1], cv::IMREAD_COLOR);
    Mat n = m.clone();
    // for debugging use the macro PRINT_MAT_INFO to print the info about the matrix, like size and type
    //PRINT_MAT_INFO(m);
    //PRINT_MAT_INFO(n);

    // 1) in order to call kmeans we need to first convert the image into floats (CV_32F)
    // see the method Mat.convertTo()

    m.convertTo(m, CV_32F);
    //PRINT_MAT_INFO(m);


    // 2) kmeans asks for a mono-dimensional list of "points". Our "points" are the pixels of the image that can be seen as 3D points
    // where each coordinate is one of the color channel (e.g. R, G, B). But they are organized as a 2D table, we need
    // to re-arrange them into a single vector.
    // see the method Mat.reshape(), it is similar to matlab's reshape
    m = m.reshape(3, n.rows*n.cols);
    //PRINT_MAT_INFO(m);

    // now we can call kmeans(...)
    Mat Labels, centers;
    clock_t start1 = clock();
    kmeans( m, k, Labels, TermCriteria() , 5 ,KMEANS_RANDOM_CENTERS , centers  );
    clock_t end1 = clock();
    double elapsed1 = double(end1 - start1) / CLOCKS_PER_SEC; 
    cout << "Temps d'exécution : " << elapsed1 << " secondes" << endl;

    //PRINT_MAT_INFO(Labels);

    //PRINT_MAT_INFO(centers);
    centers = centers.reshape(3, k); 
    centers.convertTo(centers, CV_8UC3);
    //PRINT_MAT_INFO(centers);
    // Create a new image where each pixel's color is the color of its cluster center
    Mat segmentedImage = Mat::zeros(n.size(), n.type());

    //PRINT_MAT_INFO(segmentedImage);

    Labels = Labels.reshape(1, n.rows);
    Labels.convertTo(Labels, CV_8UC1);
    //PRINT_MAT_INFO(Labels);


    for (int i = 0; i < n.rows; ++i)
    {
        for (int j = 0; j < n.cols; ++j)
        {
            // Assign the pixel the color of the corresponding center
            segmentedImage.at<Vec3b>(i, j) = centers.at<Vec3b>(Labels.at<uchar>(i, j));
            if (k==2)
            {
                if (segmentedImage.at<Vec3b>(i, j) == centers.at<Vec3b>(0))
                {
                    segmentedImage.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
                }
                else
                {
                    segmentedImage.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
                }
            }
        }
    }


    if (!groundTruthFilename.empty()) {
        Mat comparaison = imread(argv[3] , cv :: IMREAD_COLOR) ;
        //PRINT_MAT_INFO(comparaison);  // même type que segmentedImage lets go

        double FP = 0, FN = 0, VP = 0, VN = 0;

        for (int i = 0; i < n.rows; ++i)
        {
            for (int j = 0; j < n.cols; ++j)
            {
                if (comparaison.at<Vec3b>(i,j)==Vec3b(255, 255, 255) && segmentedImage.at<Vec3b>(i,j) ==Vec3b(255, 255, 255) )
                {
                    VP=VP+1;
                }
                else if (comparaison.at<Vec3b>(i,j)==Vec3b(0, 0, 0) && segmentedImage.at<Vec3b>(i,j) ==Vec3b(0, 0, 0) )
                {
                    VN=VN+1;
                }  
                else if (comparaison.at<Vec3b>(i,j)==Vec3b(255, 255, 255) && segmentedImage.at<Vec3b>(i,j) ==Vec3b(0, 0, 0) )
                {
                    FP=FP+1;
                }        
                else if (comparaison.at<Vec3b>(i,j)==Vec3b(0, 0, 0) && segmentedImage.at<Vec3b>(i,j) ==Vec3b(255, 255, 255) )
                {
                    FN=FN+1;
                }                              
            }
        }

        double P = 0, S= 0 , DSC=0 ; 

        P = VP / (VP + FP); 
        S=  VP / (VP + FN) ;
        DSC = 2*VP / (2*VP + FP + FN); 


        //cout << " VP: " << VP << endl;
        //cout << " VN: " << VN << endl;
        //cout << " FP " << FP << endl;
        //cout << " FN " << FN << endl;
        cout << " P: " << P << endl;
        cout << " S " << S << endl;
        cout << " DSC " << DSC << endl;
    }

    // Convert back to 8-bit image for display
    segmentedImage.convertTo(segmentedImage, CV_8U);

    // Display the segmented image
    imwrite( "/home/jacques/projet_image/projet_image_1/tpTI-v2024.0.0-rc1/tpTI-v2024.0.0-rc1/data/images/segmentedImage.jpg",segmentedImage);
    imshow("Segmented Image", segmentedImage);
    waitKey(0);  // Wait for a key press before closing the window

    ////////////////////////////////////// mon kmoyennes //////////////////////////////
    int N = 2;
    Mat image;
    image = imread(argv[1], cv::IMREAD_COLOR);


    Mat hist;
    int histSize = 256;  // 256 niveaux d'intensité (0 à 255)
    float range[] = {0, 256};
    const float* histRange = {range};

    int c ; 
    c= 0 ; 

    // Canaux à analyser : Bleu (0)
    /*
    void cv::calcHist	(	
    const Mat * 	images,   const veux dire que le paramètre ne peut pas être modifié 
    c'est l'adresse où est stocké image 
    int 	nimages, nombre images 
    const int * 	channels, 
    adresse de c 
    InputArray 	mask,
    OutputArray 	hist,
    int 	dims,
    const int * 	histSize,
    const float ** 	ranges,
    bool 	uniform = true,
    bool 	accumulate = false 
    )	
    */
    clock_t start = clock();
    calcHist(&image, 1, &c, Mat(), hist, 1, &histSize, &histRange, true, false);

    // Assurer que hist est en format CV_32F
    hist.convertTo(hist, CV_32F);

    // Trouver la valeur maximale et son index
    double maxVal1 = 0, maxVal2 = 0;
    Point maxIdx1, maxIdx2;

    minMaxLoc(hist, 0, &maxVal1, 0, &maxIdx1);

    cout << "Premier max du canal = " << maxVal1 << " à l'intensité " << maxIdx1.y << endl;
    // Rechercher un deuxième maximum éloigné d'au moins 50 niveaux
    for (int i = 0; i < histSize; i++) {
    float val = hist.at<float>(i);
    // condition 1 parcours deuxième max condition 2 deux pics pas trop proches
    if (val > maxVal2 && abs(i - maxIdx1.y) > 50) { 
        maxVal2 = val;
        maxIdx2.y = i;
    }
    }

    cout << "Deuxième max du canal = " << maxVal2 << " à l'intensité " << maxIdx2.y << endl;


 

    Vec3d centre1 = Vec3d(maxIdx1.y, maxIdx1.y, maxIdx1.y);  // trouvé avec cout L184
    Vec3d centre2 = Vec3d(maxIdx2.y, maxIdx2.y, maxIdx2.y);  


    // je créé un dico pour stocker les éléments de mes deux catégories
    std::map<char, std::vector<int>> dictionnaire1;

    // Boucle qui s'arrête quand les centres ne bougent plus trop 
    float dtest1 = 100, dtest2 = 100;

    // créé l'image segmenté que je vais afficher
    Mat segmentedImage_mine = Mat::zeros(image.size(), image.type());
    //PRINT_MAT_INFO(segmentedImage_mine);
    //compteur stop itération si jamais converge pas
    int compteur = 0;

    

    while ((dtest1 > 1 || dtest2 > 1 ) && compteur < 30)
    {
        compteur++;
        dictionnaire1.clear();
        dictionnaire1['n'] = {};
        dictionnaire1['b'] = {};

        //cout << " compteur: " << compteur << endl;

        for (int i = 0; i < image.rows; ++i)
        {
            for (int j = 0; j < image.cols; ++j)
            {   //cout<< " pixels : " << image.at<Vec3b>(i, j) << endl;
                float d1 = norm(Vec3d(image.at<Vec3b>(i, j)) - centre1);
                float d2 = norm(Vec3d(image.at<Vec3b>(i, j)) - centre2);

                if (d1 < d2)
                {
                    dictionnaire1['n'].push_back(i);
                    dictionnaire1['n'].push_back(j);
                }
                else
                {
                    dictionnaire1['b'].push_back(i);
                    dictionnaire1['b'].push_back(j);
                }
            }
        }
        //cout<< " dico n : " << dictionnaire1['n'].size() << endl;
        //cout<< " dico b : " << dictionnaire1['b'].size() << endl;
        
        // sert à calculer la distance pour l'interruption
        Vec3d centreold1 = centre1;
        //cout<< " centreold1 : " << centreold1 << endl;
        Vec3d centreold2 = centre2;

        // sert à créer le nouveau centre
        Vec3d centrenouveau1=Vec3d(0,0,0);
        Vec3d centrenouveau2=Vec3d(0,0,0);

        // Mise à jour des centres
    for (int i = 0; i < dictionnaire1['n'].size(); i += 2)
    {
        int x = dictionnaire1['n'][i];
        int y = dictionnaire1['n'][i + 1];

        centrenouveau1[0] = centrenouveau1[0]+Vec3d(image.at<Vec3b>(x, y))[0];
        centrenouveau1[1] = centrenouveau1[1]+Vec3d(image.at<Vec3b>(x, y))[1];
        centrenouveau1[2] = centrenouveau1[2]+Vec3d(image.at<Vec3b>(x, y))[2];
        //cout<< " centrenouveau1 : " << centrenouveau1 << endl;

        segmentedImage_mine.at<Vec3b>(x, y) = Vec3b(255, 255, 255);
    }

        // Division pour recalculer le centre
        centrenouveau1[0] = centrenouveau1[0]/(dictionnaire1['n'].size() / 2);
        centrenouveau1[1] = centrenouveau1[1]/(dictionnaire1['n'].size() / 2);
        centrenouveau1[2] = centrenouveau1[2]/(dictionnaire1['n'].size() / 2);
        centre1=centrenouveau1;
        //cout<< " centre1 : " << centre1 << endl;

        for (int i = 0; i < dictionnaire1['b'].size(); i += 2)
        {
            int x = dictionnaire1['b'][i];
            int y = dictionnaire1['b'][i + 1];

            centrenouveau2[0] = centrenouveau2[0]+Vec3d(image.at<Vec3b>(x, y))[0];
            centrenouveau2[1] = centrenouveau2[1]+Vec3d(image.at<Vec3b>(x, y))[1];
            centrenouveau2[2] = centrenouveau2[2] +Vec3d(image.at<Vec3b>(x, y))[2];

            segmentedImage_mine.at<Vec3b>(x, y) = Vec3b(0, 0, 0);
        }

        // Division pour recalculer le centre
        centrenouveau2[0] = centrenouveau2[0]/(dictionnaire1['b'].size() / 2);
        centrenouveau2[1] = centrenouveau2[1]/(dictionnaire1['b'].size() / 2);
        centrenouveau2[2] = centrenouveau2[2]/(dictionnaire1['b'].size() / 2);

        centre2=centrenouveau2;

        dtest1 = norm(centre1 - centreold1);
        //cout<< " dtest1 : " << dtest1 << endl;
        dtest2 = norm(centre2 - centreold2);
        //cout<< " dtest2 : " << dtest2 << endl;
    }

    clock_t end = clock();

    double elapsed = double(end - start) / CLOCKS_PER_SEC; 
    cout << "Temps d'exécution du mien: " << elapsed << " secondes" << endl;

    if (!groundTruthFilename.empty())
    {
        Mat comparaison = imread(argv[3], cv::IMREAD_COLOR);
        //PRINT_MAT_INFO(comparaison);

        double FPmine = 0, FNmine = 0, VPmine = 0, VNmine = 0;

        for (int i = 0; i < image.rows; ++i)
        {
            for (int j = 0; j < image.cols; ++j)
            {
                Vec3b comp = comparaison.at<Vec3b>(i, j);
                Vec3b seg = segmentedImage_mine.at<Vec3b>(i, j);

                if (comp == Vec3b(255, 255, 255) && seg == Vec3b(255, 255, 255))
                    VPmine++;
                else if (comp == Vec3b(0, 0, 0) && seg == Vec3b(0, 0, 0))
                    VNmine++;
                else if (comp == Vec3b(255, 255, 255) && seg == Vec3b(0, 0, 0))
                    FPmine++;
                else if (comp == Vec3b(0, 0, 0) && seg == Vec3b(255, 255, 255))
                    FNmine++;
            }
        }

        double Pmine = 0, Smine= 0 , DSCmine=0 ; 

        Pmine = VPmine / (VPmine + FPmine); 
        Smine=  VPmine / (VPmine + FNmine) ;
        DSCmine = 2*VPmine / (2*VPmine + FPmine + FNmine); 

    cout << " VPmine : " << VPmine << endl;
    cout << " VNmine : " << VNmine << endl;
    cout << " FPmine : " << FPmine << endl;
    cout << " FNmine : " << FNmine << endl;
    cout << " Pmine : " << Pmine << endl;
    cout << " Smine : " << Smine << endl;
    cout << " DSCmine : " << DSCmine << endl;
}

// Convert back to 8-bit image for display
segmentedImage_mine.convertTo(segmentedImage_mine, CV_8U);

// Display the segmented image
imwrite("/home/jacques/projet_image/projet_image_1/tpTI-v2024.0.0-rc1/tpTI-v2024.0.0-rc1/data/images/segmentedImage_mine.jpg", segmentedImage_mine);
imshow("Segmented Image mine", segmentedImage_mine);
waitKey(0);

return EXIT_SUCCESS;

}
