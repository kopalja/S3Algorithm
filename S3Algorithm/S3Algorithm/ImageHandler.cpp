#include "stdafx.h"
#include "ImageHandler.h"
#include "Image.h"

#include <Windows.h>
#include <Wincodecsdk.h>
#include <iostream>
using namespace std;


ImageHandler::ImageHandler( char* inputPath, char* outputPath, HRESULT* hr )
{
	*hr = DefineFormat( inputPath );
	if ( SUCCEEDED( hr ) )
	{
		wchar_t *wStringIn = new wchar_t[256];
		wchar_t *wStringOut = new wchar_t[256];
		*hr = SetPaths( inputPath, outputPath, wStringIn, wStringOut );
		if ( SUCCEEDED( hr ) )
		{
			*hr = SetCoders( (LPCWSTR)wStringIn, (LPCWSTR)wStringOut );
			if ( SUCCEEDED( hr ) )
			{
				*hr = SetFrames();
			}
		}
		delete[] wStringIn;
		delete[] wStringOut;
	}
}


HRESULT ImageHandler::DefineFormat( char *inputPath )
{
	int i = 0;
	while (true)
	{
		if ( inputPath[i] == '.' ) break;
		i++;
	}
	if ( ( inputPath[i + 1] == 'j' && inputPath[i + 2] == 'p' && inputPath[i + 3] == 'g' ) || 
		( inputPath[i + 1] == 'J' && inputPath[i + 2] == 'P' && inputPath[i + 3] == 'G' ) ) 
	{
		format = Format::Jpeg;
		byteDepth = 3;
	}
	else if ( ( inputPath[i + 1] == 't' && inputPath[i + 2] == 'i' && inputPath[i + 3] == 'f' ) || 
		( inputPath[i + 1] == 'T' && inputPath[i + 2] == 'I' && inputPath[i + 3] == 'F' ) )
	{
		format = Format::Tiff;
		byteDepth = 4;
	}
	else 
	{
		return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT ImageHandler::SetPaths( __in char* inputPath, __in char* outputPath, __out wchar_t *wStringIn, __out wchar_t *wStringOut )
{
	try
	{
		MultiByteToWideChar(CP_ACP, 0, inputPath, -1, wStringIn, 256);
		MultiByteToWideChar(CP_ACP, 0, outputPath, -1, wStringOut, 256);
	}
	catch ( exception )
	{
		return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT ImageHandler::SetCoders( __in LPCWSTR inputPath, __in LPCWSTR outputPath ) 
{
	HRESULT hr = S_OK;
	// Initialize COM.
	hr = CoInitializeEx(NULL, COINITBASE_MULTITHREADED);
	IWICImagingFactory *piFactory = NULL;
	piDecoder = NULL;

	// Create the COM imaging factory.
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&piFactory));
		// Create the decoder.
		if (SUCCEEDED(hr))
		{
			hr = piFactory->CreateDecoderFromFilename( inputPath, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &piDecoder );
			// Variables used for encoding.
			piFileStream = NULL;
			piEncoder = NULL;
			piBlockWriter = NULL;
			piBlockReader = NULL;
			WICPixelFormatGUID pixelFormat = { 0 }; 
			dpiX, dpiY = 0.0;
			width, height = 0;
			// Create a file stream.
			if ( SUCCEEDED( hr ) )
			{
				hr = piFactory->CreateStream(&piFileStream);
				// Initialize our new file stream.
				if ( SUCCEEDED( hr ) )
				{
					hr = piFileStream->InitializeFromFilename( outputPath, GENERIC_WRITE );
					// Create the encoder.
					if ( SUCCEEDED( hr ) )
					{
						if ( format == Format::Jpeg )
							hr = piFactory->CreateEncoder(GUID_ContainerFormatJpeg, NULL, &piEncoder); 
						else if ( format == Format::Tiff )
							hr = piFactory->CreateEncoder(GUID_ContainerFormatTiff, NULL, &piEncoder); 
						else if ( format == Format::Wrong )
							hr = E_INVALIDARG;

						// Initialize the encoder
						if ( SUCCEEDED( hr ) )
						{
							hr = piEncoder->Initialize(piFileStream, WICBitmapEncoderNoCache);
							if ( SUCCEEDED( hr ) )
							{
								hr = piDecoder->GetFrameCount( 0 );
							}
						}
					}
				}
			}
		}
	}
	return hr;
}

HRESULT ImageHandler::SetFrames()
{
	HRESULT hr = S_OK;
	// Frame variables.
	piFrameDecode = NULL;
	piFrameEncode = NULL;
	piFrameQReader = NULL;
	piFrameQWriter = NULL;

	// Get and create the image frame.
	if (SUCCEEDED(hr))
	{
		hr = piDecoder->GetFrame(0, &piFrameDecode);
		if (SUCCEEDED(hr))
		{
			hr = piEncoder->CreateNewFrame(&piFrameEncode, NULL);
			// Initialize the encoder.
			if (SUCCEEDED(hr))
			{
				hr = piFrameEncode->Initialize(NULL);
				// Get and set the size.
				if (SUCCEEDED(hr))
				{
					hr = piFrameDecode->GetSize(&width, &height);
					UINT outWidth = ( width + 8 - ( width % 8 ) ) / 4;
					UINT outHeight = ( height + 8 - ( height % 8 ) ) / 4;
					if ( width % 8 == 0 ) outWidth -= 2;
					if ( height % 8 == 0 ) outHeight -= 2;
					if (SUCCEEDED(hr))
					{

						hr = piFrameEncode->SetSize( outWidth, outHeight );
						// Get and set the resolution.
						if (SUCCEEDED(hr))
						{
							WICPixelFormatGUID k;
							hr = piFrameDecode->GetPixelFormat( &k );
							if ( k == GUID_WICPixelFormat24bpp3Channels || k == GUID_WICPixelFormat24bppBGR || k == GUID_WICPixelFormat24bppRGB )
								byteDepth = 3;
							else if ( k == GUID_WICPixelFormat32bpp3ChannelsAlpha || 
								 k == GUID_WICPixelFormat32bpp4Channels || 
								 k == GUID_WICPixelFormat32bppBGR ||
								 k == GUID_WICPixelFormat32bppBGR101010 ||
								 k == GUID_WICPixelFormat32bppBGRA ||
								 k == GUID_WICPixelFormat32bppCMYK ||
								 k == GUID_WICPixelFormat32bppGrayFixedPoint ||
								 k == GUID_WICPixelFormat32bppGrayFloat ||
								 k == GUID_WICPixelFormat32bppPBGRA ||
								 k == GUID_WICPixelFormat32bppPRGBA ||
								 k == GUID_WICPixelFormat32bppRGB ||
								 k == GUID_WICPixelFormat32bppRGBA ||
								 k == GUID_WICPixelFormat32bppRGBA1010102 ||
								 k == GUID_WICPixelFormat32bppRGBA1010102XR ||
								 k == GUID_WICPixelFormat32bppRGBE ) 
								 byteDepth = 4;
							else 
								hr = E_INVALIDARG;

							piFrameDecode->GetResolution(&dpiX, &dpiY);
							if (SUCCEEDED(hr))
							{
								hr = piFrameEncode->SetResolution(dpiX, dpiY);
								// Set the pixel format.
								if (SUCCEEDED(hr))
								{
									piFrameDecode->GetPixelFormat(&pixelFormat);
									if (SUCCEEDED(hr))
									{
										WICPixelFormatGUID p = GUID_WICPixelFormat8bppGray;
										hr = piFrameEncode->SetPixelFormat(&p);
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return hr;
}

HRESULT ImageHandler::Create( __out Image* image )
{
	WICRect wRect = { 0, 0, width, height };
	HRESULT hr = piFrameDecode->CopyPixels( &wRect, image->width * byteDepth, image->size, image->buffer );
	return hr;
}

HRESULT ImageHandler::Save( __in Image *image ) 
{
	HRESULT hr = piFrameEncode->WritePixels( image->height, image->width, image->height * image->width, image->buffer );

	// Check that the destination format and source formats are the same.
	bool formatsEqual = FALSE;
	if (SUCCEEDED(hr))
	{
		GUID srcFormat;
		GUID destFormat;
		hr = piDecoder->GetContainerFormat(&srcFormat);
		if (SUCCEEDED(hr))
		{
			hr = piEncoder->GetContainerFormat(&destFormat);
			if (SUCCEEDED(hr))
			{
				if (srcFormat == destFormat)
					formatsEqual = true;
				else
					formatsEqual = false;

				if (SUCCEEDED(hr) && formatsEqual)
				{
					// Copy metadata using metadata block reader/writer.
					if (SUCCEEDED(hr))
					{
						hr = piFrameDecode->QueryInterface(IID_PPV_ARGS(&piBlockReader));
						if (SUCCEEDED(hr))
						{
							hr = piFrameEncode->QueryInterface(IID_PPV_ARGS(&piBlockWriter));
							if (SUCCEEDED(hr))
							{
								hr = piBlockWriter->InitializeFromBlockReader(piBlockReader);
								if (SUCCEEDED(hr))
								{
									hr = piFrameEncode->GetMetadataQueryWriter(&piFrameQWriter);
									if (SUCCEEDED(hr))
									{
										// Add additional metadata.
										PROPVARIANT    value;
										value.vt = VT_LPWSTR;
										value.pwszVal = L"Metadata Test Image.";
										if ( format == Format::Tiff )
											hr = piFrameQWriter->SetMetadataByName(L"/ifd/xmp/dc:title", &value);
										if (SUCCEEDED(hr))
										{
											hr = piFrameEncode->WriteSource(
												static_cast<IWICBitmapSource*> (piFrameDecode),
												NULL); // Using NULL enables JPEG loss-less encoding.

											// Commit the frame.
											if (SUCCEEDED(hr))
											{
												hr = piFrameEncode->Commit();
												if (piFrameDecode)
												{
													hr = piFrameDecode->Release();
												}

												if (piFrameEncode)
												{
													piFrameEncode->Release();
												}

												if (piFrameQReader)
												{
													piFrameQReader->Release();
												}

												if (piFrameQWriter)
												{
													piFrameQWriter->Release();
												}
												if (SUCCEEDED(hr))
												{
													hr = piEncoder->Commit();
													if (SUCCEEDED(hr))
													{
														piFileStream->Commit( STGC_DEFAULT );
														if (piFileStream)
														{
															piFileStream->Release();
														}
														if (piEncoder)
														{
															piEncoder->Release();
														}
														if (piBlockWriter)
														{
															piBlockWriter->Release();
														}
														if (piBlockReader)
														{
															piBlockReader->Release();
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return hr;
}

