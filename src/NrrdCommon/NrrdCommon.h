#pragma once

#include "NrrdIO.h"

#include <msclr\marshal_cppstd.h>
#include <map>

using namespace System;
using System::Collections::Generic::Dictionary;
using System::IntPtr;
using System::Runtime::InteropServices::Marshal;
typedef Nrrd cNrrd;
typedef NrrdAxisInfo cNrrdAxisInfo;

namespace NrrdCommon {
	public enum class NrrdType: int // from NrrdIO.h
	{
		nrrdTypeUnknown = 0,     /*  0: signifies "type is unset/unknown" */
		nrrdTypeDefault = 0,     /*  0: signifies "determine output type for me" */
		nrrdTypeChar,          /*  1:   signed 1-byte integer */
		nrrdTypeUChar,         /*  2: unsigned 1-byte integer */
		nrrdTypeShort,         /*  3:   signed 2-byte integer */
		nrrdTypeUShort,        /*  4: unsigned 2-byte integer */
		nrrdTypeInt,           /*  5:   signed 4-byte integer */
		nrrdTypeUInt,          /*  6: unsigned 4-byte integer */
		nrrdTypeLLong,         /*  7:   signed 8-byte integer */
		nrrdTypeULLong,        /*  8: unsigned 8-byte integer */
		nrrdTypeFloat,         /*  9:          4-byte floating point */
		nrrdTypeDouble,        /* 10:          8-byte floating point */
		nrrdTypeBlock,         /* 11: size user defined at run time; MUST BE LAST */
		nrrdTypeLast
	};

	public enum class NrrdKind : int // from NrrdIO.h
	{
		nrrdKindUnknown,
		nrrdKindDomain,            /*  1: any image domain */
		nrrdKindSpace,             /*  2: a spatial domain */
		nrrdKindTime,              /*  3: a temporal domain */
		/* -------------------------- end domain kinds */
		/* -------------------------- begin range kinds */
		nrrdKindList,              /*  4: any list of values, non-resample-able */
		nrrdKindPoint,             /*  5: coords of a point */
		nrrdKindVector,            /*  6: coeffs of (contravariant) vector */
		nrrdKindCovariantVector,   /*  7: coeffs of covariant vector (eg gradient) */
		nrrdKindNormal,            /*  8: coeffs of unit-length covariant vector */
		/* -------------------------- end arbitrary size kinds */
		/* -------------------------- begin size-specific kinds */
		nrrdKindStub,              /*  9: axis with one sample (a placeholder) */
		nrrdKindScalar,            /* 10: effectively, same as a stub */
		nrrdKindComplex,           /* 11: real and imaginary components */
		nrrdKind2Vector,           /* 12: 2 component vector */
		nrrdKind3Color,            /* 13: ANY 3-component color value */
		nrrdKindRGBColor,          /* 14: RGB, no colorimetry */
		nrrdKindHSVColor,          /* 15: HSV, no colorimetry */
		nrrdKindXYZColor,          /* 16: perceptual primary colors */
		nrrdKind4Color,            /* 17: ANY 4-component color value */
		nrrdKindRGBAColor,         /* 18: RGBA, no colorimetry */
		nrrdKind3Vector,           /* 19: 3-component vector */
		nrrdKind3Gradient,         /* 20: 3-component covariant vector */
		nrrdKind3Normal,           /* 21: 3-component covector, assumed normalized */
		nrrdKind4Vector,           /* 22: 4-component vector */
		nrrdKindQuaternion,        /* 23: (w,x,y,z), not necessarily normalized */
		nrrdKind2DSymMatrix,       /* 24: Mxx Mxy Myy */
		nrrdKind2DMaskedSymMatrix, /* 25: mask Mxx Mxy Myy */
		nrrdKind2DMatrix,          /* 26: Mxx Mxy Myx Myy */
		nrrdKind2DMaskedMatrix,    /* 27: mask Mxx Mxy Myx Myy */
		nrrdKind3DSymMatrix,       /* 28: Mxx Mxy Mxz Myy Myz Mzz */
		nrrdKind3DMaskedSymMatrix, /* 29: mask Mxx Mxy Mxz Myy Myz Mzz */
		nrrdKind3DMatrix,          /* 30: Mxx Mxy Mxz Myx Myy Myz Mzx Mzy Mzz */
		nrrdKind3DMaskedMatrix,    /* 31: mask Mxx Mxy Mxz Myx Myy Myz Mzx Mzy Mzz */
		nrrdKindLast
	};

	public value struct NrrdAxisInfo
	{
	public:
		NrrdAxisInfo(cNrrdAxisInfo ai)
		{
			Centre = ai.center;
			Kind = (NrrdKind)ai.kind;
			Size = (int)ai.size;
			Min = ai.min;
			Max = ai.max;
			Spacing = ai.spacing;
			Thickness = ai.thickness;
			Label = gcnew String(ai.label);
			Units = gcnew String(ai.units);
		}

		int Centre, Size;
		double Min, Max, Spacing, Thickness;
		String^ Label, ^Units;
		NrrdKind Kind;
	};

	public ref class Nrrd
	{
	public:
		Nrrd()
		{
			mNrrd = NULL;
		}

		~Nrrd()
		{
			nrrdNuke(mNrrd);
		}

		static Nrrd^ Load(String^ filename)
		{
			char * err;

			cNrrd* nin;

			nin = nrrdNew();

			Console::WriteLine(String::Format("airMyQNaNHiBit {0}", airMyQNaNHiBit));

			std::string nativeFilename = msclr::interop::marshal_as<std::string>(filename);

			if (nrrdLoad(nin, nativeFilename.c_str(), NULL))
			{
				err = biffGetDone(NRRD);
				String^ errorString = gcnew String(err);
				free(err);

				throw gcnew Exception(errorString);
			}

			Nrrd^ nrrd = gcnew Nrrd(nin);
			nrrd->Axes = gcnew array<NrrdAxisInfo>(nin->dim);

			for (int i = 0; i < nin->dim; ++i)
			{
				nrrd->Axes[i] = NrrdAxisInfo(nin->axis[i]);
			}

			return nrrd;
		}

		property NrrdType Type
		{
			NrrdType get()
			{
				if (mNrrd != NULL)
					return (NrrdType)mNrrd->type;
				return NrrdType::nrrdTypeUnknown;
			}

		}

		property String^ SpaceUnits
		{
			String^ get()
			{
				if (mNrrd != NULL)
					return gcnew String(*(mNrrd->spaceUnits), 0, 8);
				return String::Empty;
			}

		}

		property String^ SampleUnits
		{
			String^ get()
			{
				if (mNrrd != NULL)
					return gcnew String(mNrrd->sampleUnits);
				return String::Empty;
			}
		}

		property array<double>^ SpaceOrigin
		{
			array<double>^ get()
			{
				if (mNrrd != NULL)
				{
					array<double>^ values = gcnew array<double>(8);
					for (int i = 0; i < values->Length; ++i)
						values[i] = mNrrd->spaceOrigin[i];
					return values;
				}
				return gcnew array<double>(0);
			}
		}

		property int SpaceDimension
		{
			int get()
			{
				if (mNrrd != NULL)
					return (int)mNrrd->spaceDim;
				return 0;
			}
		}

		generic <typename T>
		array<T>^ Data()
		{
			if (mNrrd != NULL)
			{
				size_t totalSize = mNrrd->axis[0].size;
				for (int i = 1; i < mNrrd->dim; ++i)
				{
					totalSize *= mNrrd->axis[i].size;
				}

				unsigned int structSize = sizeof(T);

				if (structSize != mTypeSize)
					throw gcnew System::Exception(System::String::Format("Size of type doesn't match data. Size of type: {0}. Expected size: {1}", structSize, mTypeSize));
				
				array<T>^ data = gcnew array<T>(totalSize);
				char* ptr = (char*)mNrrd->data;

				for (int i = 0; i < data->Length; ++i)
				{
					data[i] = Marshal::PtrToStructure<T>((IntPtr)ptr);
					ptr = ptr + structSize;
				}

				//Marshal::Copy((IntPtr)mNrrd->data, data, 0, totalSize);

				return data;
			}
			return gcnew array<T>(0);
		}

		property Dictionary<String^, String^>^ UserValues
		{
			Dictionary<String^, String^>^ get()
			{
				if (mNrrd == NULL)
					return gcnew Dictionary<String^, String^>();

				int numKvp = nrrdKeyValueSize(mNrrd);

				Dictionary<String^, String^>^ dict = gcnew Dictionary<String^, String^>();

				for (int i = 0; i < numKvp; ++i)
				{
					char* v = nrrdKeyValueGet(mNrrd, mNrrd->kvp[i]);
					dict->Add(gcnew String(mNrrd->kvp[i]), gcnew String(v));
				}

				return dict;
			}
		}

		property long TotalCount
		{
			long get()
			{
				size_t totalSize = mNrrd->axis[0].size;
				for (int i = 1; i < mNrrd->dim; ++i)
				{
					totalSize *= mNrrd->axis[i].size;
				}

				return  (long)totalSize;
			}
		}

		property String^ Content
		{
			String^ get()
			{
				if (mNrrd != NULL)
					return gcnew String(mNrrd->content);
				return String::Empty;
			}
		}

		property int Dimensions
		{
			int get()
			{
				if (mNrrd != NULL)
					return (int)mNrrd->dim;
				return 0;
			}
		}

		IntPtr GetPointer()
		{
			return IntPtr(mNrrd);
		}

		array<NrrdAxisInfo>^ Axes;

	private:
		cNrrd* mNrrd;
		int mTypeSize;

		void SetTypeSize()
		{
			std::map<std::string, int> typeSizeMap {
				{"signed char", 1},
				{ "int8", 1 },
				{ "int8_t", 1 },
				{ "uchar", 1 },
				{ "unsigned char", 1 },
				{ "uint8", 1 },
				{ "uint8_t", 1 },

				{ "short", 2 },
				{ "short int", 2 },
				{ "signed short", 2 },
				{ "signed short int", 2 },
				{ "int16", 2 },
				{ "int16_t", 2 },
				{ "ushort", 2 },
				{ "unsigned short", 2 },
				{ "unsigned short int", 2 },
				{ "uint16", 2 },
				{ "uint16_t", 2 },

				{ "int", 4 },
				{ "signed int", 4 },
				{ "int32", 4 },
				{ "int32_t", 4 },
				{ "uint", 4 },
				{ "unsigned int", 4 },
				{ "uint32", 4 },
				{ "uint32_t", 4 },

				{ "longlong", 8 },
				{ "long long", 8 },
				{ "long long int", 8 },
				{ "signed long long", 8 },
				{ "signed long long int", 8 },
				{ "int64", 8 },
				{ "int64_t", 8 },
				{ "ulonglong", 8 },
				{ "unsigned long long", 8 },
				{ "unsigned long long int", 8 },
				{ "uint64", 8 },
				{ "uint64_t", 8 },

				{ "float", 4 },
				{ "double", 8 },
				{ "block", mNrrd->blockSize }
			};

			switch (mNrrd->type)
			{
			case(nrrdTypeChar):
			case(nrrdTypeUChar):
				mTypeSize = 1;
				break;
			case(nrrdTypeShort):
			case(nrrdTypeUShort):
				mTypeSize = 2;
				break;
			case(nrrdTypeInt):
			case(nrrdTypeUInt):
			case(nrrdTypeFloat):
				mTypeSize = 4;
				break;
			case(nrrdTypeLLong):
			case(nrrdTypeULLong):
			case(nrrdTypeDouble):
				mTypeSize = 8;
				break;
			case(nrrdTypeBlock):
				mTypeSize = mNrrd->blockSize;
				break;
			default:
				throw gcnew System::Exception("Invalid or unknown data type.");
			}

		}

		Nrrd(cNrrd* ptr)
		{
			mNrrd = ptr;
			SetTypeSize();
		}

		Nrrd(IntPtr ptr)
		{
			mNrrd = (cNrrd*)ptr.ToPointer();
			SetTypeSize();
		}
	};
}
