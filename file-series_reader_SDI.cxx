////program to use itk to convert between file-formats, streaming version
//01: based on file_converter_SDI.cxx and http://www.itk.org/Wiki/ITK/Examples/IO/VolumeFromSlices


#include <complex>

#include "itkFilterWatcher.h" 
#include <itkNumericSeriesFileNames.h>
#include <itkImageSeriesReader.h>
#include <itkImageFileWriter.h>




int dispatch_cT(itk::ImageIOBase::IOPixelType, itk::ImageIOBase::IOComponentType, size_t, int, char **);

template<typename InputComponentType>
int dispatch_pT(itk::ImageIOBase::IOPixelType pixelType, size_t, int, char **);

template<typename InputComponentType, typename InputPixelType>
int dispatch_D(size_t, int, char **);

template<typename InputComponentType, typename InputPixelType, size_t Dimension>
int DoIt(int, char *argv[]);





template<typename InputComponentType, typename InputPixelType, size_t Dimension>
int DoIt(int argc, char *argv[]){

    typedef InputPixelType  OutputPixelType;
    
    typedef itk::Image<InputPixelType, Dimension>  InputImageType;
    typedef itk::Image<OutputPixelType, Dimension>  OutputImageType;

    typedef itk::NumericSeriesFileNames    NameGeneratorType;
    NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
 
    nameGenerator->SetSeriesFormat(argv[1]);
    nameGenerator->SetStartIndex(atoi(argv[3]));
    nameGenerator->SetEndIndex(atoi(argv[4]));
    nameGenerator->SetIncrementIndex(1);
    std::vector<std::string> names = nameGenerator->GetFileNames();

    // List the files
    for(unsigned int i = 0; i < names.size(); ++i)
	std::cerr << "File: " << names[i] << std::endl;
 
    typedef itk::ImageSeriesReader<InputImageType> ReaderType;
    typename ReaderType::Pointer reader = ReaderType::New();
 
    ////reading compressed MHA/MHD is supported for streaming!
    reader->SetFileNames(names);
    reader->UseStreamingOn(); //optional, default: On

    typedef itk::ImageFileWriter<OutputImageType>  WriterType;
    typename WriterType::Pointer writer = WriterType::New();

    FilterWatcher watcherO(writer);
    writer->SetFileName(argv[2]);
    writer->SetInput(reader->GetOutput());
    writer->UseCompressionOff(); //writing compressed is not supported for streaming!
    writer->SetNumberOfStreamDivisions(atoi(argv[5]));
    try{ 
        writer->Update();
        }
    catch(itk::ExceptionObject &ex){ 
        std::cerr << ex << std::endl;
        return EXIT_FAILURE;
        }

    return EXIT_SUCCESS;

    }


int dispatch_cT(itk::ImageIOBase::IOComponentType componentType, itk::ImageIOBase::IOPixelType pixelType, size_t dimensionType, int argc, char *argv[]){
  int res= 0;

  //http://www.itk.org/Doxygen45/html/classitk_1_1ImageIOBase.html#a8dc783055a0af6f0a5a26cb080feb178
  //http://www.itk.org/Doxygen45/html/itkImageIOBase_8h_source.html#l00107
  //IOComponentType: UNKNOWNCOMPONENTTYPE, UCHAR, CHAR, USHORT, SHORT, UINT, INT, ULONG, LONG, FLOAT, DOUBLE

  switch (componentType){
  case itk::ImageIOBase::UCHAR:{        // uint8_t
    typedef unsigned char InputComponentType;
    res= dispatch_pT<InputComponentType>(pixelType, dimensionType, argc, argv);
  } break;
  case itk::ImageIOBase::CHAR:{         // int8_t
    typedef char InputComponentType;
    res= dispatch_pT<InputComponentType>(pixelType, dimensionType, argc, argv);
  } break;
  case itk::ImageIOBase::USHORT:{       // uint16_t
    typedef unsigned short InputComponentType;
    res= dispatch_pT<InputComponentType>(pixelType, dimensionType, argc, argv);
  } break;
  case itk::ImageIOBase::SHORT:{        // int16_t
    typedef short InputComponentType;
    res= dispatch_pT<InputComponentType>(pixelType, dimensionType, argc, argv);
  } break;
  case itk::ImageIOBase::UINT:{         // uint32_t
    typedef unsigned int InputComponentType;
    res= dispatch_pT<InputComponentType>(pixelType, dimensionType, argc, argv);
  } break;
  case itk::ImageIOBase::INT:{          // int32_t
    typedef int InputComponentType;
    res= dispatch_pT<InputComponentType>(pixelType, dimensionType, argc, argv);
  } break;
  case itk::ImageIOBase::ULONG:{        // uint64_t
    typedef unsigned long InputComponentType;
    res= dispatch_pT<InputComponentType>(pixelType, dimensionType, argc, argv);
  } break;
  case itk::ImageIOBase::LONG:{         // int64_t
    typedef long InputComponentType;
    res= dispatch_pT<InputComponentType>(pixelType, dimensionType, argc, argv);
  } break;
  case itk::ImageIOBase::FLOAT:{        // float32
    typedef float InputComponentType;
    res= dispatch_pT<InputComponentType>(pixelType, dimensionType, argc, argv);
  } break;
  case itk::ImageIOBase::DOUBLE:{       // float64
    typedef double InputComponentType;
    res= dispatch_pT<InputComponentType>(pixelType, dimensionType, argc, argv);
  } break;
  case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
  default:
    std::cerr << "unknown component type" << std::endl;
    break;
  }//switch
  return res;
}

template<typename InputComponentType>
int dispatch_pT(itk::ImageIOBase::IOPixelType pixelType, size_t dimensionType, int argc, char *argv[]){
  int res= 0;
    //http://www.itk.org/Doxygen45/html/classitk_1_1ImageIOBase.html#abd189f096c2a1b3ea559bc3e4849f658
    //http://www.itk.org/Doxygen45/html/itkImageIOBase_8h_source.html#l00099
    //IOPixelType:: UNKNOWNPIXELTYPE, SCALAR, RGB, RGBA, OFFSET, VECTOR, POINT, COVARIANTVECTOR, SYMMETRICSECONDRANKTENSOR, DIFFUSIONTENSOR3D, COMPLEX, FIXEDARRAY, MATRIX 

  switch (pixelType){
  case itk::ImageIOBase::SCALAR:{
    typedef InputComponentType InputPixelType;
    res= dispatch_D<InputComponentType, InputPixelType>(dimensionType, argc, argv);
  } break;
  case itk::ImageIOBase::RGB:{
    typedef itk::RGBPixel<InputComponentType> InputPixelType;
    res= dispatch_D<InputComponentType, InputPixelType>(dimensionType, argc, argv);
  } break;
  case itk::ImageIOBase::RGBA:{
    typedef itk::RGBAPixel<InputComponentType> InputPixelType;
    res= dispatch_D<InputComponentType, InputPixelType>(dimensionType, argc, argv);
  } break;
  case itk::ImageIOBase::COMPLEX:{
    typedef std::complex<InputComponentType> InputPixelType;
    res= dispatch_D<InputComponentType, InputPixelType>(dimensionType, argc, argv);
  } break;
  case itk::ImageIOBase::VECTOR:{
    typedef itk::VariableLengthVector<InputComponentType> InputPixelType;
    res= dispatch_D<InputComponentType, InputPixelType>(dimensionType, argc, argv);
  } break;
  case itk::ImageIOBase::UNKNOWNPIXELTYPE:
  default:
    std::cerr << std::endl << "Error: Pixel type not handled!" << std::endl;
    break;
  }//switch 
  return res;
}


template<typename InputComponentType, typename InputPixelType>
int dispatch_D(size_t dimensionType, int argc, char *argv[]){
  int res= 0;
  switch (dimensionType){
  case 1:
    res= DoIt<InputComponentType, InputPixelType, 1>(argc, argv);
    break;
  case 2:
    res= DoIt<InputComponentType, InputPixelType, 2>(argc, argv);
    break;
  case 3:
    res= DoIt<InputComponentType, InputPixelType, 3>(argc, argv);
    break;
  default: 
    std::cerr << "Error: Images of dimension " << dimensionType << " are not handled!" << std::endl;
    break;
  }//switch 
  return res;
}



////from http://itk-users.7.n7.nabble.com/Pad-image-with-0-but-keep-its-type-what-ever-it-is-td27442.html
//namespace itk{
  // Description:
  // Get the PixelType and ComponentType from fileName

void GetImageType (std::string fileName, itk::SizeValueType start,
    itk::ImageIOBase::IOPixelType &pixelType,
    itk::ImageIOBase::IOComponentType &componentType,
    size_t &dimensionType
    ){
    typedef itk::NumericSeriesFileNames    NameGeneratorType;
    NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
 
    nameGenerator->SetSeriesFormat(fileName.c_str());
    nameGenerator->SetStartIndex(start);

    typedef itk::Image<char, 1> ImageType; //template initialization parameters need to be given but can be arbitrary here
    itk::ImageFileReader<ImageType>::Pointer imageReader= itk::ImageFileReader<ImageType>::New();
    imageReader->SetFileName(nameGenerator->GetFileNames()[0]);
    imageReader->UpdateOutputInformation();

    pixelType = imageReader->GetImageIO()->GetPixelType(); // segfaults if ImageSeriesReader is used for imageReader
    componentType = imageReader->GetImageIO()->GetComponentType();
    dimensionType= imageReader->GetImageIO()->GetNumberOfDimensions() + 1;

    std::cerr << std::endl << "dimensions: " << dimensionType << std::endl;  
    std::cerr << "component type: " << imageReader->GetImageIO()->GetComponentTypeAsString(componentType) << std::endl;
    std::cerr << "component size: " << imageReader->GetImageIO()->GetComponentSize() << std::endl; 
    std::cerr << "pixel type (string): " << imageReader->GetImageIO()->GetPixelTypeAsString(imageReader->GetImageIO()->GetPixelType()) << std::endl; 
    std::cerr << "pixel type: " << pixelType << std::endl << std::endl; 

    }
  


int main(int argc, char *argv[]){
    if ( argc != 6 ){
	std::cerr << "Missing Parameters: "
		  << argv[0]
		  << " Input_Image-pattern"
		  << " Output_Image"
		  << " first last"
		  << " stream-chunks"
    		  << std::endl;

	return EXIT_FAILURE;
	}

    itk::ImageIOBase::IOPixelType pixelType;
    typename itk::ImageIOBase::IOComponentType componentType;
    size_t dimensionType;


    try {
        GetImageType(argv[1], atoi(argv[3]), pixelType, componentType, dimensionType);
        }//try
    catch( itk::ExceptionObject &excep){
        std::cerr << argv[0] << ": exception caught !" << std::endl;
        std::cerr << excep << std::endl;
        return EXIT_FAILURE;
        }
 
    return dispatch_cT(componentType, pixelType, dimensionType, argc, argv);
    }





