/*===========================
Chronophage Games (2011)

BaTemplate.h

Author(s) : 
	* Frédéric Brachfogel

===========================*/

template<class T>
class TItem
{
protected:
	BYTE	Item[ sizeof(T) ];
	BOOL	bHaveOne;

public:
	TItem():
		bHaveOne( FALSE )
	{}

	TItem( const TItem<T> &In ):
		bHaveOne( FALSE )
	{
		if( In.bHaveOne )
			Add( In );
	}

	TItem( const T &In ):
		bHaveOne( FALSE )
	{
		Add( In );
	}

	void Add( const T &In )
	{
		Delete();
		bHaveOne = TRUE;
		new(Item) T(In);
	}

	void Delete()
	{
		if( bHaveOne )
		{
			bHaveOne = FALSE;
			((T*)Item)->~T();
		}
	}

	BOOL HasOne()const
	{
		return bHaveOne;
	}

	const T& GetItem()const
	{
		ASSERT( bHaveOne )
		return *(T*)Item;
	}

	T& GetItem()
	{
		ASSERT( bHaveOne )
		return *(T*)Item;
	}

	TItem<T>& operator=( const TItem<T> &In )
	{
		ASSERT( this != &In )
		if( In.bHaveOne )
			Add( In );
		return *this;
	}

	operator const T&()const
	{
		return GetItem();
	}

	operator T&()
	{
		return GetItem();
	}
};

template<typename T>
BOutStream& operator<<( BOutStream &InStream, const TItem<T> &InItem )
{
	InStream << InItem.HasOne();
	if( InItem.HasOne() )
		InStream << InItem.GetItem();
	return InStream;
}

template<typename T>
BInStream& operator>>( BInStream &InStream, TItem<T> &OutItem )
{
	BOOL bHaveOne;
	InStream >> bHaveOne;
	if( bHaveOne )
	{
		T Item;
		InStream >> Item;
		OutItem.Add( Item );
	}
	return InStream;
}

//=========================//

struct BClassType
{
	enum 
	{ 
		EIsAtomic = 0
	};
};

struct BAtomicType
{
	enum 
	{ 
		EIsAtomic = 1
	};
};

struct BClassBehavior
{
	enum 
	{ 
		ENeedConstructor = 1 
	};
};

struct BAtomicTypeBehavior
{
	enum 
	{ 
		ENeedConstructor = 0 
	};
};

template<typename T> 
struct TTypeInfos : public BClassType, public BClassBehavior {};

template<> struct TTypeInfos<VOID*> : public BAtomicType, public BAtomicTypeBehavior {};
template<> struct TTypeInfos<BYTE> : public BAtomicType, public BAtomicTypeBehavior {};
template<> struct TTypeInfos<ANSICHAR> : public BAtomicType, public BAtomicTypeBehavior {};
template<> struct TTypeInfos<UNICHAR> : public BAtomicType, public BAtomicTypeBehavior {};
template<> struct TTypeInfos<SHORT> : public BAtomicType, public BAtomicTypeBehavior {};
template<> struct TTypeInfos<INT> : public BAtomicType, public BAtomicTypeBehavior {};
template<> struct TTypeInfos<UNSIGNED INT> : public BAtomicType, public BAtomicTypeBehavior {};
template<> struct TTypeInfos<LONG> : public BAtomicType, public BAtomicTypeBehavior {};
template<> struct TTypeInfos<UNSIGNED LONG> : public BAtomicType, public BAtomicTypeBehavior {};
template<> struct TTypeInfos<FLOAT> : public BAtomicType, public BAtomicTypeBehavior {};
template<> struct TTypeInfos<DOUBLE> : public BAtomicType, public BAtomicTypeBehavior {};

//=========================//

template<typename T>
class TArray
{
protected:
	VOID	*Data;
	INT		Count, 
			Size;

public:
	struct BIterator
	{
		T *Data;
		INT Index;

		BIterator():
			Data( 0 ),
			Index( 0 ) 
		{}

		BIterator( T *InData, INT InIndex ):
			 Data( InData),
			 Index(InIndex)
		{}

		BIterator( const BIterator &It ):
		    Data( It.Data ),
			Index( It.Index )
		{}

		operator T*()const
		{
			ASSERT( Data );
			return &Data[Index];
		}

		T* operator->()const
		{
			ASSERT( Data );
			return &Data[Index];
		}

		operator INT&()
		{
			return Index;
		}

		operator INT()const
		{
			return Index;
		}

		BOOL operator==( const BIterator &It )const
		{
			return Index == It.Index;
		}

		BOOL operator!=( const BIterator &It )const
		{
			return Index != It.Index;
		}

		BOOL operator<( const BIterator &It )const
		{
			return Index < It.Index;
		}

		BOOL operator<=( const BIterator &It )const
		{
			return Index <= It.Index;
		}

		BOOL operator>( const BIterator &It )const
		{
			return Index > It.Index;
		}

		BOOL operator>=( const BIterator &It )const
		{
			return Index >= It.Index;
		}

		BIterator& operator=( const BIterator &In )
		{
			Index = In.Index;
			Data = In.Data;
			return *this;
		}

		BIterator& operator=( const INT &In )
		{
			Index = In;
			return *this;
		}

		BIterator& operator=( T *InData )
		{
			SLOW_ASSERT(Data != InData);
			Data = InData;
			return *this;
		}
	};

	TArray():
		Data( 0 ),
		Count( 0 ),
		Size( 0 )
	{}

	TArray( INT InSize, INT InCount ):
		Data( 0 ),
		Count( 0 ),
		Size( 0 )
	{
		if( InSize )
		{
			Alloc( InSize );
			SetCount( InCount );
		}
	}

	TArray( const TArray<T> &InArray ):
		Data( 0 ),
		Count( 0 ),
		Size( 0 )
	{
		Copy( InArray );
	}

	TArray( T *InPtr, INT InSize, INT InCount ):
		Data( InPtr ),
		Size( InSize ),
		Count( InCount )
	{}

	virtual ~TArray()
	{
		Free();
	}

	virtual void Copy( const TArray<T> &InArray )
	{
		if( &InArray != this )
		{
			TArray<T>::Free();
			if(InArray.Size)
			{
				Alloc(InArray.Size);
				if( InArray.Count )
				{
					Count = InArray.Count;

					if(TTypeInfos<T>::ENeedConstructor)
						for(INT F = 0 ; F < Count ; F++)
							new( (T*)Data + F ) T( InArray[F] );
					else
						MemCopy( 0, InArray, InArray.Count);
				}
			}
		}
	}

	void Setup( T *InPtr, INT InSize, INT InCount = 0 )
	{
		ASSERT( InCount <= InSize )

		Free();
		Data = InPtr;
		Size = InSize;
		Count = InCount;
	}

	virtual void MemSet( INT In, INT StartIndex = 0, INT InCount = 0 )
	{
		ASSERT( StartIndex>=0 && StartIndex+InCount<=Count )
		ASSERT( InCount>=0 )

		if( !InCount )
			InCount = Count;
		
		gMemSet( (T*)Data + StartIndex, In, InCount*sizeof(T) );
	}

	virtual void MemCopy( INT StartIndex, const TArray<T> &InArray, INT InCount )
	{
		ASSERT( StartIndex>=0 && StartIndex+InCount<=Count );
		ASSERT( InCount>=0 );

		gMemCpy( (T*)Data + StartIndex, InArray.Data, sizeof(T)*InCount );
	}

	virtual void MemCopy( INT StartIndex, const T *InArray, INT InCount )
	{
		ASSERT( StartIndex>=0 && StartIndex+InCount<=Count );
		ASSERT( InCount>=0 );

		gMemCpy( (T*)Data + StartIndex, InArray, sizeof(T)*InCount );
	}

	BOOL IsIndexValid( INT Index )const
	{
		return (Index>=0 && Index<Count);
	}

	BOOL IsEmpty()const
	{
		return Count == 0;
	}

	BOOL IsAlloc()const
	{
		return Data != 0;
	}

	virtual void Free()
	{
		Clear();

		delete[] Data;
		Data = 0;
		Size = 0;
	}

	virtual void Clear()
	{
		SetCount( 0, TRUE );
	}

	virtual INT Add( const T &InElement )
	{
		if( Count + 1 > Size )
			ReAlloc( Size + 1 );

		if( TTypeInfos<T>::ENeedConstructor )
			new( (T*)Data+Count ) T( InElement );
		else
			((T*)Data)[Count] = InElement;

		Count++;

		return Count - 1;
	}

	virtual void Add( const T &InElement, INT Index )
	{
		ASSERT( Index >= 0);

		if( Index >= Size )
			ReAlloc( Index + 1 );

		if( Index >= Count )
		{
			if( TTypeInfos<T>::ENeedConstructor )
				for( INT F = Count ; F < Index+1 ; F++ )
					new( (T*)Data + F ) T;
            Count = Index + 1;
		}
		else
            ShiftRight( Index );

		((T*)Data)[Index] = InElement;
	}

	virtual void Add( const TArray<T> &InArray, INT Index = ERROR_INDEX )
	{
		if( Index == ERROR_INDEX )
			Index = Count;

		if( Index + InArray.Count > Size )
			ReAlloc( Index + InArray.Count );

		if( Index >= Count )
		{
			if( TTypeInfos<T>::ENeedConstructor )
				for( INT F = Count ; F < Index + InArray.Count ; F++ )
					new( (T*)Data + F ) T;
            Count = Index + InArray.Count;
		}
		else
			ShiftRight( Index, InArray.Count );

		if( TTypeInfos<T>::ENeedConstructor )
			for( INT F = 0 ; F < InArray.Count ; F++ )
				((T*)Data)[Index+F] = InArray[F];
		else
			gMemCpy( (T*)Data+Index, InArray, InArray.Count*sizeof(T) );
	}

	virtual INT AddSingle( const T &InElement )
	{
		BIterator I = Find( InElement );

		if( I == End() )
			return TArray<T>::Add( InElement );

		// Re-add it for the needs of multipairs.
		((T*)Data)[ I.Index ] = InElement;

		return I.Index;
	}

	virtual void ShiftRight( INT Index = 0, INT ShiftCount = 1 )
	{
		ASSERT( ShiftCount > 0 );
		ASSERT( IsIndexValid(Index) );
		
		if( Count + ShiftCount > Size )
			ReAlloc( Count + ShiftCount );

		if( TTypeInfos<T>::ENeedConstructor )
		{
			for( INT F = 0 ; F < ShiftCount ; F++ )
				new( (T*)Data+Count+F ) T;
			Count += ShiftCount;
			for( INT F = Count-1 ; F >= Index+ShiftCount ; F-- )
				((T*)Data)[F] = ((T*)Data)[F-ShiftCount];
		}
		else
		{
			INT CopyCount = Count-Index;
			Count += ShiftCount;

			gMemMove( (T*)Data + Index + ShiftCount, sizeof(T)*CopyCount, (T*)Data + Index, sizeof(T)*CopyCount );
		}
	}

	virtual void ShiftLeft( INT Index = 0, INT ShiftCount = 1)
	{
		ASSERT( IsIndexValid(Index) );
		ASSERT( ShiftCount > 0 );
		
		if( TTypeInfos<T>::ENeedConstructor )
		{
			for( INT F = (Index-ShiftCount < 0 ? 0 : Index-ShiftCount) ; F < Count-ShiftCount ; F++ )
				((T*)Data)[F] = ((T*)Data)[F+ShiftCount];
			INT Diff = Count-ShiftCount < 0 ? Count : ShiftCount;
			for( INT F = 0 ; F < Diff ; F++ )
				((T*)Data)[Count-1-F].~T();
		}
		else
		{
			INT Start = Index-ShiftCount < 0 ? 0 : Index-ShiftCount;
			gMemMove( (T*)Data+Start, sizeof(T)*(Count-ShiftCount-Start), (T*)Data+Start+ShiftCount, sizeof(T)*(Count-ShiftCount-Start) );
		}

		Count = Count-ShiftCount < 0 ? 0 : Count-ShiftCount;
	}

	void Replace( const T &Old, const T &New )
	{
		for( BIterator Iter = Begin() ; Iter != End() ; ++Iter )
			if( *Iter == Old )
				*Iter = New;
	}

	virtual void DeleteAt( INT Index )
	{
		ASSERT( IsIndexValid(Index) );
		ASSERT( Index < Count );

		if( Index+1 < Count )
			ShiftLeft( Index + 1 );
		else
		{
			Count--;
			((T*)Data)[Count].~T();
		}
	}

	void DeleteAll( const T &InElement )
	{
		BIterator I;
		while( (I = Find(InElement)) != End() )
			DeleteAt( I.Index );
	}

	BIterator Delete( const T & InElement )
	{
		BIterator I;
		if( (I = Find(InElement)) != End() )
			DeleteAt(I.Index);

		return I;
	}

	INT GetSize()const
	{
		return Size;
	}

	INT GetCount()const
	{
		return Count;
	}

	void IncreaseSize( INT InSize )
	{
		ReAlloc( Size + InSize );
	}

	void DecreaseSize( INT InSize )
	{
		ReAlloc( Size - InSize );
	}

	void IncreaseCount( INT InCount = 1 )
	{
		SetCount( Count + InCount );
	}

	void DecreaseCount( INT InCount = 1 )
	{
		SetCount( Count - InCount );
	}

	INT LastIndex()const
	{
		return Count-1;
	}

	const T& operator[]( INT Index )const
	{
		SLOW_ASSERT( IsIndexValid(Index) );
		return ((T*)Data)[Index];
	}

	T& operator[]( INT Index )
	{
		SLOW_ASSERT( IsIndexValid(Index) );
		return ((T*)Data)[Index];
	}

	operator T*()const
	{ 
		return (T*)Data; 
	}

	TArray<T>& operator=( const TArray<T> &In )
	{ 
		SLOW_ASSERT( this != &In )

        Copy( In );

		return *this; 
	}

	virtual void Alloc( INT InSize )
	{
		ASSERT( InSize>0 );
		ASSERT( !Data );
		
		Size = InSize;
		Data = (VOID*)new BYTE[Size*sizeof(T)];

		SLOW_ASSERT( Data );
	}

	virtual void ReAlloc( INT InSize, BOOL bDiscard = FALSE )
	{
		ASSERT( InSize > 0 );

		if( Size == InSize )
		{
			if( bDiscard )
				Clear();
		}
		else
		if( Size == 0 )
		{
			Alloc( InSize );
		}
		else
		{
			/*INT ByteCount = InSize * sizeof( T );
			BOOL bExtend = ( gGetMainMemoryAllocator().IsAllocateFromHere( Data ) && gGetMainMemoryAllocator().ExtendTo( Data, ByteCount ) )
						|| ( gGetOneClusterMemoryAllocator().IsAllocateFromHere( Data ) && gGetOneClusterMemoryAllocator().ExtendTo( Data, ByteCount ) );
			
			if( bExtend )
			{
				Size = InSize;
				if( bDiscard )
					SetCount( 0, TRUE );
			}
			else*/
			{
				if( !bDiscard )
				{
					ASSERT( Count <= InSize )

					TArray<T> TempArray( *this );

					TArray<T>::Free();
					Alloc( InSize );

					Count = TempArray.Count;

					if( Count )
						if( TTypeInfos<T>::ENeedConstructor )
							for( INT F = 0 ; F < Count ; F++ )
								new( (T*)Data+F ) T( TempArray[F] );
						else
							MemCopy( 0, TempArray, Count );
				}
				else
				{
					Free();
					Alloc( InSize );
				}
			}
		}
	}

	void Fit()
	{
		if( Count )
			ReAlloc( Count );
		else
			Free();
	}

	virtual void SetCount( INT NewCount, BOOL bDiscard = FALSE )
	{
		ASSERT( NewCount >= 0 )

		if( NewCount > Size )
			ReAlloc( NewCount, bDiscard );

		if( TTypeInfos<T>::ENeedConstructor )
		{
			if( bDiscard )
			{
				for( INT F = 0 ; F < Count ; F++ )
					((T*)Data)[F].~T();

				Count = 0;
			}
			else
			{
				for( INT F = NewCount ; F < Count ; F++ )
					((T*)Data)[F].~T();
			}

			for( INT F = Count ; F < NewCount ; F++ )
				new( (T*)Data+F ) T;
		}

		Count = NewCount;
	}

	virtual void GetSubArray( TArray<T> &OutArray, INT StartIndex, INT EndIndex = INIT_NEG )const
	{
		if( EndIndex == INIT_NEG )
			EndIndex = Count;

		ASSERT( IsIndexValid(StartIndex) );
		ASSERT( EndIndex > StartIndex && EndIndex <= Count );

		INT ItemCount = EndIndex-StartIndex;
		OutArray.SetCount( ItemCount );

		if( TTypeInfos<T>::ENeedConstructor )
			for(INT F = StartIndex ; F < EndIndex ; F++)
				OutArray[F] = ((T*)Data)[F];
		else
			OutArray.MemCopy( 0, (T*)Data+StartIndex, ItemCount );
	}

	void Swap( TArray<T> &Array )
	{
		TSwap( Data, Array.Data );
		TSwap( Count, Array.Count );
		TSwap( Size, Array.Size );
	}

	void Reverse()
	{
		INT Half = Count / 2,
			Last = Count - 1;

		for( INT I = 0 ; I < Half ; I++ )
			TSwap<T>( ((T*)Data)[I], ((T*)Data)[Last-I] );
	}

	virtual T*& GetData()
	{ 
		return (T*&)Data; 
	}

	virtual T* GetData()const
	{ 
		return (T*)Data; 
	}

	BIterator Begin()const
	{
		return BIterator( (T*)Data, 0 );
	}

	BIterator End()const
	{
		return BIterator( (T*)Data, Count );
	}

	BIterator Find( const T &InElement )const
	{
		for( BIterator Iter = Begin() ; Iter != End() ; ++Iter )
			if( InElement == ((T*)Data)[ Iter ] )
				return Iter;
		return End();
	}

	BIterator FindAt( const T &InElement, INT Index )const
	{
		for( BIterator Iter = BIterator((T*)Data, Index) ; Iter != End() ; ++Iter )
			if( InElement == ((T*)Data)[ Iter ] )
				return Iter;
		return End();
	}

	BIterator FindFromEnd( const T &InElement )const
	{
		for( INT F = Count-1 ; F >= 0 ; F-- )
			if( ((T*)Data)[F] == InElement )
				return BIterator( (T*)Data, F );

		return End();
	}

	BOOL operator==( const TArray<T> &Array )const
	{
		BOOL bResult = Count == Array.Count;

		if( bResult )
			for( INT F = 0 ; F < Count ; F++ )
				if( ((T*)Data)[F] != Array[F] )
				{
					bResult = FALSE;
					break;
				}

		return bResult;
	}

	BOOL operator!=( const TArray<T> &Array )const
	{
		return !( *this == Array );
	}

	INT Hash()const
	{
		return gHashFunction( (BYTE*)Data, Count*sizeof(T) );
	}

	BIterator Last()const
	{
		return BIterator( (T*)Data, Count-1 );
	}

	void QuickSort( BIterator FirstIt, BIterator LastIt )
	{
		ASSERT( FirstIt != End() && LastIt != End() )
		
		if( (INT)LastIt-FirstIt == 1 )
		{
			if( *FirstIt > *LastIt )
				TSwap( *FirstIt, *LastIt );
		}
		else 
		if( FirstIt < LastIt )
		{
			T Pivot = ChoosePivot( FirstIt, LastIt );
			
			BIterator It = Split( Pivot, FirstIt, LastIt );
			
			if( *It < Pivot )
			{
				QuickSort( FirstIt, It );
				It++;
				QuickSort( It, LastIt );
			}
			else
			if( *It > Pivot )
			{
				QuickSort( It, LastIt );
				It--;
				QuickSort( FirstIt, It );
			}	
			else 
			if( It == FirstIt )
			{
				It++;
				QuickSort( It, LastIt );
			}
			else 
			if( It == LastIt )
			{
				It--;
				QuickSort( FirstIt, It );
			}
		}
	}

protected:
	T ChoosePivot( BIterator FirstIt, BIterator LastIt )
	{
		INT A = gGetRandom( FirstIt.Index, LastIt.Index ),
			B = gGetRandom( FirstIt.Index, LastIt.Index ),
			C = gGetRandom( FirstIt.Index, LastIt.Index );

		if( GetData()[A] > GetData()[B] )
		{
			if( GetData()[B] > GetData()[C] )
				return GetData()[B];
			else
				return gMin( GetData()[A], GetData()[C] );
		}
		else
		{
			if( GetData()[A] > GetData()[C] )
				return GetData()[A];
			else
				return gMin( GetData()[B], GetData()[C] );
		}
	}

	BIterator Split( const T &Pivot, BIterator FirstIt, BIterator LastIt )
	{
		while( FirstIt != LastIt )
		{
			while( *FirstIt < Pivot )
			{
				FirstIt++;

				if( FirstIt == LastIt )
					return FirstIt;
			}
				
			while( *LastIt > Pivot )
			{
				LastIt--;

				if( FirstIt == LastIt )
					return FirstIt;
			}

			TSwap( *FirstIt, *LastIt );
			
			if( (INT)LastIt-FirstIt == 1 )
				return FirstIt;
			
			FirstIt++;
			LastIt--;
		}

		return FirstIt;
	}
};

template<typename T>
BOutStream& operator<<( BOutStream &InStream, const TArray<T> &InArray )
{
	InStream << InArray.GetSize();
	InStream << InArray.GetCount();

	if( InArray.GetCount() )
		if( TTypeInfos<T>::EIsAtomic )
			InStream.Write( InArray, InArray.GetCount()*sizeof(T) );
		else
			for( INT F = 0 ; F < InArray.GetCount() ; F++ )
				InStream << InArray[F];

	return InStream;
}

template<typename T>
BInStream& operator>>( BInStream &InStream, TArray<T> &OutArray )
{
	INT Size, Count;

	OutArray.Clear();

	InStream >> Size;
	InStream >> Count;

	ASSERT( Count <= Size );

	if( Size > 0 )
	{
		OutArray.ReAlloc( Size, TRUE );
		
		if( Count > 0 )
		{
			OutArray.SetCount( Count );

			if( TTypeInfos<T>::EIsAtomic && !TTypeInfos<T>::ENeedConstructor )
				InStream.Read( OutArray, Count*sizeof(T) );
			else
				for( INT F = 0 ; !InStream.Eos() && F < Count ; F++ )
					InStream >> OutArray[F];
		}
	}

	return InStream;
}

//=========================//

template<class T>
struct TInterfacedArray : public TArray<T>
{
	TInterfacedArray()
	{}

	TInterfacedArray( T *InPtr, INT InSize, INT InCount = 0 ):
		TArray<T>( InPtr, InSize, InCount )
	{}

	virtual ~TInterfacedArray()
	{
		Free();
	}

	virtual void Free()
	{
		Data = 0;
		Count = 0;
		Size = 0;
	}

	virtual void Alloc( INT InSize )
	{
		FATAL_ERROR( TEXT("TInterfacedArray::Alloc"), TEXT("This action is not allowed") )
	}

	virtual void Copy( const TArray<T> &InArray )
	{
		Setup( InArray.GetData(), InArray.GetSize(), InArray.GetCount() );
	}
}; 

//=========================//

template<class T>
struct TSingleArray : public TArray<T>
{
	TSingleArray( INT InSize = 0, INT InCount = 0):
		TArray<T>( InSize, InCount )
	{}

	TSingleArray( const TArray<T> &InArray ):
		TArray<T>()
	{
		Add( InArray, 0 );
	}

	virtual INT Add( const T &InElement )
	{
		return AddSingle( InElement );
	}

	virtual void Add( const T &InElement, INT Index )
	{
		if( Find(InElement) == End() )
			TArray<T>::Add( InElement, Index );
	}

	virtual void Add( const TArray<T> &InArray, INT Index = ERROR_INDEX )
	{	
		if( Index == ERROR_INDEX )
			Index = Count;

		INT I = 0;
		for( INT F = 0 ; F < InArray.GetCount() ; F++ )
			if( Find( InArray[F] ) == End() )
			{
				Add( InArray[F], Index+I );
				I++;
			}
	}
};

//=========================//

template<class T>
class TBlockArray : public TArray<T>
{
protected:
	UNSIGNED INT BlockSize;

public:
	TBlockArray( INT InSize = 0, INT InCount = 0 ):
		TArray<T>( InSize, InCount ),
		BlockSize( BLOCK_ARRAY_BLOCK_SIZE )
	{}

	TBlockArray( const TArray<T> &InArray ):
		TArray<T>( InArray ),
		BlockSize( BLOCK_ARRAY_BLOCK_SIZE )
	{}

	TBlockArray( T *InPtr, INT InSize, INT InCount = 0 ):
		TArray<T>( InPtr, InSize, InCount ),
		BlockSize( BLOCK_ARRAY_BLOCK_SIZE )
	{}

	void SetBlockSize( INT NewBlockSize )
	{
		BlockSize = NewBlockSize;
	}

	UNSIGNED INT GetBlockSize()const
	{
		return BlockSize;
	}

	virtual void ReAlloc( INT InSize, BOOL bDiscard = FALSE )
	{
		TArray<T>::ReAlloc( InSize + BlockSize, bDiscard );
	}
};

//=========================//

template<typename T>
struct TVirtualRefArray : public TArray<T*>
{
private:
	TVirtualRefArray( const TVirtualRefArray<T> &InArray )
	{}

	virtual void MemSet( INT In, INT StartIndex = 0, INT InCount = 0 )
	{}

	virtual void MemCopy( INT StartIndex, const TArray<T> &InArray, INT InCount )
	{}

	virtual void MemCopy( INT StartIndex, const T *InArray, INT InCount )
	{}

	virtual void Copy( const TArray<T> &InArray )
	{}

	TVirtualRefArray<T>& operator=( const TVirtualRefArray<T> &InArray )
	{
		return *this;
	}

	virtual void GetSubArray( TArray<T*> &, INT, INT = INIT_NEG )const
	{}

public:
	TVirtualRefArray( INT InSize = 0, INT InCount = 0 ):
		TArray<T*>( InSize, InCount )
	{
		if( Size )
			gMemSet( Data, 0, Size*sizeof(T*) );
	}

	virtual ~TVirtualRefArray()
	{
		Free();
	}

	virtual void Free()
	{
		for( INT F = 0 ; F < Count ; F++ )
			delete ((T**)Data)[F];

		TArray<T*>::Free();
	}

	// There is an obscur compiler(MSVC) reason wich imply to reimplement this function.
	virtual INT Add( T *InElement )
	{
		if( Count+1 > Size )
			ReAlloc( Size + 1 );

		INT Result = Count;

		((T**)Data)[Count++] = InElement;

		return Result;
	}

	virtual void Add( T *InElement, INT Index )
	{
		INT Temp = Count;

		TArray<T*>::Add( InElement, Index );

		if( Index+1 > Temp )
			for( INT I = Temp ; I < Index ; I++ )
				((T**)Data)[I] = 0;
	}

	virtual void ShiftRight( INT Index = 0, INT ShiftCount = 1 )
	{
		TArray<T*>::ShiftRight( Index, ShiftCount );

		for( INT F = Index ; F < Index+ShiftCount ; F++ )
			((T**)Data)[F] = 0;
	}

	virtual void ShiftLeft( INT Index = 0, INT ShiftCount = 1 )
	{
		ASSERT( IsIndexValid(Index) );
		ASSERT( ShiftCount>=0 );
		
		for( INT F = (Index-ShiftCount < 0 ? 0 : Index-ShiftCount) ; F < Index + (Index-ShiftCount >= 0 ? 0 : ShiftCount-Index) ; F++ )			
			delete ((T**)Data)[F];

		TArray<T*>::ShiftLeft( Index, ShiftCount );
	}

	virtual void Alloc( INT InSize )
	{
		ASSERT( InSize>0 );
		ASSERT( !Data );
		
		Size = InSize;
		Data = new BYTE[ Size * sizeof(T*) ];

		gMemSet( Data, 0, Size * sizeof(T*) );

		SLOW_ASSERT( Data );
	}

	virtual void DeleteAt( INT Index )
	{
		ASSERT( IsIndexValid(Index) );

		if( Index+1 < Count )
			ShiftLeft( Index + 1 );
		else
		{
			Count--;
			delete ((T**)Data)[ Count ];
		}
	}
};

//=========================//

template<typename T>
struct TRefArray : public TVirtualRefArray<T>
{
	TRefArray( INT InSize = 0, INT InCount = 0 ):
		TVirtualRefArray<T>( InSize, InCount )
	{}

	virtual void Copy( const TRefArray<T> &InArray )
	{
		if( &InArray != this )
		{
			Free();
			if( InArray.Size )
			{
				Alloc( InArray.Size );
				Count = InArray.Count;

				for( INT F = 0 ; F < InArray.Count ; F++ )
					if( InArray[F] )
						((T**)Data)[F] = new T(*InArray[F]);
			}
		}
	}

	TRefArray<T>& operator=( const TRefArray<T> &In )
	{
		TArray<T*>::operator=( In );
		return *this; 
	}
};

//=========================//

template<typename T>
struct TIndexedArray : public TArray<T>
{
protected:
	mutable INT Index;

public:
	TIndexedArray( INT InSize = 0, INT InCount = 0 ):
		TArray<T>( InSize, InCount ),
		Index( ERROR_INDEX )
	{}

	TIndexedArray( const TIndexedArray<T> &InArray ):
		TArray<T>( InArray ),
		Index( 0 )
	{}

	TIndexedArray( T *InPtr, INT InSize, INT InCount=0 ):
		TArray<T>( InPtr, InSize, InCount ),
		Index( 0 )
	{}

	TIndexedArray( INT InSize ):
		TArray<T>( InSize ),
		Index( ERROR_INDEX )
	{}

	void SetIndex( INT I )
	{
		Index = I;
	}

	INT GetIndex()const
	{
		return Index;
	}

	void SetNext( INT InCount = 1 )
	{
		ASSERT( InCount > 0);
		Index += InCount;
	}

	void SetPrev( INT InCount = 1 )
	{
		ASSERT( InCount > 0);
		Index -= InCount;
	}

	void Reset()
	{
		Index = 0;
	}

	BOOL IsAtEnd()const
	{
		return Index == GetCount();
	}

	T& GetValue()
	{
		return operator[]( Index );
	}

	const T& GetValue()const
	{
		return operator[]( Index );
	}

	void SetValue( const T &Value )
	{
		operator[]( Index ) = Value;
	}
};

//=========================//

template<class T>
struct TInterfacedIndexedArray : public TIndexedArray<T>
{
	TInterfacedIndexedArray()
	{}

	TInterfacedIndexedArray( T *InPtr, INT InSize, INT InCount = 0 ):
		TIndexedArray( InPtr, InSize, InCount )
	{}

	virtual ~TInterfacedIndexedArray()
	{
		Free();
	}

	virtual void Free()
	{
		Data = 0;
		Count = 0;
		Size = 0;
	}

	virtual void Alloc( INT InSize )
	{
		FATAL_ERROR( TEXT("TInterfacedIndexedArray::Alloc"), TEXT("This action is not allowed") )
	}

	virtual void Copy( const TArray<T> &InArray )
	{
		Setup( InArray.GetData(), InArray.GetSize(), InArray.GetCount() );
	}
};

//=========================//

template<class T, class TAllocator=BFreeListMemoryPool>
class TList
{
protected:
	struct BListNode : public BFreeListNode
	{
		T Data;
		BListNode *Next;

		BListNode()
			:Next( 0 )
		{}

		BListNode( const T &InData, BListNode *InNext = 0 ): 
			Next( InNext ),
			Data( InData )
		{}

		operator T*()const
		{
			return &Data;
		}

		operator T*()
		{
			return &Data;
		}

		VOID* operator new( SIZE_T Size )
		{
			TAllocator *Allocator = GetAllocator();
			BListNode *Result = (BListNode*)Allocator->New( (INT)Size );
			Result->MemoryPool = Allocator;
			return Result;
		}

		void operator delete( VOID *Ptr )
		{
			BListNode *ListNode = (BListNode*)Ptr;
			ListNode->MemoryPool->Delete( Ptr );
		}
	};

	BListNode *HeadList;
	INT Count;

	static TAllocator* GetAllocator()
	{
		static TAllocator *Allocator = new TAllocator;
		return Allocator;
	}

public:
	struct BIterator
	{
		BListNode	*PrevNode,
					*Node;
		INT			Index;

		BIterator( BListNode *InPrevNode = 0, BListNode *InNode = 0, INT InIndex = 0 ): 
		  PrevNode( InPrevNode ),
		  Node( InNode ),
		  Index( InIndex )
		{}

		BIterator( const BIterator &It ):
			PrevNode( It.PrevNode ),
			Node( It.Node ),
			Index( It.Index )
		{}

		BIterator& operator++()
		{
			SLOW_ASSERT( Node );

			PrevNode = Node;
			Node = Node->Next;
			Index++;

			return *this;
		}

		BOOL operator==( const BIterator &Iter )const
		{
			return Node == Iter.Node;
		}

		BOOL operator!=( const BIterator &Iter )const
		{
			return Node != Iter.Node;
		}

		BOOL operator<( const BIterator &Iter )const
		{
			for( BIterator It = Node ; It != 0 ; )
				if( ++It == Iter )
					return TRUE;

			return FALSE;
		}

		BOOL operator<=( const BIterator &Iter )const
		{
			for( BIterator It = Node ; It != 0 ; ++It )
				if( It == Iter )
					return TRUE;

			return FALSE;
		}

		BOOL operator>( const BIterator &Iter )const
		{
			for( BIterator It = Iter.Node ; It != 0 ; )
				if( ++It == *this )
					return TRUE;

			return FALSE;
		}

		BOOL operator>=( const BIterator &Iter )const
		{
			for( BIterator It = Iter.Node ; It != 0 ; ++It )
				if( It == *this )
					return TRUE;

			return FALSE;
		}

		operator T*()const
		{
			ASSERT( Node )
			return &(Node->Data);
		}

		T* operator->()const
		{
			ASSERT( Node )
			return &(Node->Data);
		}

		//INT cast operator missing because implies confusing casts
		INT GetIndex()const
		{
			return Index;
		}
	};

	TList():
		HeadList( 0 ), 
		Count( 0 )
	{}
	
	TList( const TList<T,TAllocator> &InList ):
		HeadList( 0 ),
		Count( 0 )
	{
		*this = InList;
	}

	virtual ~TList()
	{
		Free();
	}

	BIterator Begin()const
	{
		return BIterator( 0, HeadList, 0 );
	}

	static BIterator End() 
	{
		return BIterator( 0, 0, ERROR_INDEX );
	}

	BIterator InsertFront( const T &InData )
	{
		BListNode *NewNode = new BListNode( InData, HeadList );
		SLOW_ASSERT( NewNode );
		HeadList = NewNode;

		Count++;

		return Begin();
	}

	BIterator InsertBack( const T &InData )
	{
		BListNode *NewNode = new BListNode( InData, 0 );
		SLOW_ASSERT( NewNode );

		if( HeadList )
		{
			BIterator Iter = Begin();
			for( ; Iter.Node->Next != 0 ; ++Iter );

			Iter.Node->Next = NewNode;
			BIterator Result( Iter.Node, NewNode, Count );
			Count++;
			return Result;
		}
		
		HeadList = NewNode;
		Count++;
		return Begin();
	}

	BIterator InsertAt( INT Index, const T &InData )
	{
		ASSERT( Index < Count )
		ASSERT( Index >= 0 )

		if( Index == 0 )
			return InsertFront( InData );

        BListNode *NewNode = new BListNode( InData, 0 );
		SLOW_ASSERT( NewNode );

		BIterator Iter = GetIndex( Index );
		Iter.PrevNode->Next = NewNode;
		NewNode->Next = Iter.Node;
		Count++;

		return BIterator( Iter.PrevNode, NewNode, Index );
	}

	BIterator Insert( const BIterator &Iter, const T &InData )
	{
		BListNode *NewNode = new BListNode( InData, 0 );
		SLOW_ASSERT( NewNode );

		NewNode->Next = Iter.Node->Next;
		Iter.Node->Next = NewNode;

		Count++;
		
		return BIterator( NewNode, Iter.GetIndex() + 1 );
	}

	BIterator Insert( const BIterator &Iter, const TList<T,TAllocator> &InList )
	{
		ASSERT( &InList != this )

		TList<T,TAllocator> TempList( InList );

		if( InList.Count() )
		{
			BIterator It;
			for( BIterator Iterator = TempList.Begin() ; Iterator != TempList.End() ; ++Iterator )
				It = Iterator;

			It.Node->Next = Iter.Node->Next;
			Iter.Node->Next = TempList.GetHeadList();

			TempList.HeadList = 0;
		}

		Count += InList.GetCount();

		BIterator Result( Iter );
		
		return ++Result;
	}

	BIterator InsertFront( const TList<T,TAllocator> &InList )
	{	
		ASSERT( &InList != this )

		TList<T,TAllocator> TempList;
		for( BIterator Iter = InList.Begin() ; Iter != InList.End() ; ++Iter )
			TempList.InsertFront( *Iter );

		for( BIterator Iter = TempList.Begin() ; Iter != TempList.End() ; ++Iter )
			InsertFront( *Iter );

		return Begin();
	}

	BIterator InsertBack( const TList<T,TAllocator> &InList )
	{
		ASSERT( &InList != this )

		TList<T,TAllocator>::BIterator It;
		TList<T,TAllocator>::BIterator Iter = InList.Begin();

		if(Iter == InList.End())
			return End();
			
		It = InsertBack( *Iter );	

		for( ++Iter ; Iter != InList.End() ; ++Iter )
			InsertBack( *Iter );

		return It;
	}

	BIterator InsertFrontSingle( const T &InData )
	{
		BIterator Iter = Find( InData );

		if( Iter == End() )
			return InsertFront( InData );

		return Iter;
	}

	BIterator InsertFrontSingle( const TList<T,TAllocator> &InList )
	{
		BIterator Result = End();

		TList<T,TAllocator> TempList;
		for( BIterator Iter = InList.Begin() ; Iter != InList.End() ; ++Iter )
			TempList.InsertFront( *Iter );

		for( BIterator Iter = TempList.Begin() ; Iter != TempList.End() ; ++Iter )
			Result = InsertFrontSingle( *Iter );

		return Result;
	}

	BIterator InsertBackSingle( const T &InData )
	{
		BIterator Iter = Find(InData);

		if( Iter == End() )
			return InsertBack( InData );

		return Iter;
	}

	BIterator InsertBackSingle( const TList<T,TAllocator> &InList )
	{
		BIterator Result = End();

		for( BIterator Iter = InList.Begin() ; Iter != InList.End() ; ++Iter )
			Result = InsertBackSingle( *Iter );

		return Result;
	}

	void ChangeData( BIterator &Iter, const T &InData )
	{
		ASSERT( Iter.Node )
		Iter.Node->Data = InData;
	}

	virtual void RemoveFront()
	{
		ASSERT(HeadList);

		BListNode *Node = HeadList;
		HeadList = HeadList->Next;
		delete Node;

		Count--;
	}

	virtual void RemoveBack()
	{
		ASSERT( HeadList );

		if( HeadList->Next )
		{
			BListNode* N;

			for( N = HeadList ; N->Next->Next ; N = N->Next);

			delete N->Next;
			N->Next = 0;
		}
		else
		{
			delete HeadList;
			HeadList = 0;
		}

		Count--;
	}

	virtual void Remove( BIterator &InIter )
	{
		if( !InIter.PrevNode )
		{
			BListNode *Node = HeadList;
			HeadList = HeadList->Next;
			delete Node;

			Count--;

			InIter = Begin();
		}
		else
		{
			BListNode *Node = InIter.Node;
			InIter.PrevNode->Next = InIter.Node->Next;
			InIter.Node = InIter.Node->Next;
			delete Node;

			Count--;
		}
	}

	void Remove( const T &InData )
	{
		BIterator Iter = Find( InData );

		if( Iter != End() )
			Remove( Iter );
	}

	void RemoveAt( INT Index )
	{
		ASSERT( Index < Count )
		ASSERT( Index >= 0 ) 

		Count--;

		if( Index == 0 )
		{
			RemoveFront();
			return;
		}

		BIterator Iter = Begin();
		for( INT I = 0; I+1 < Index ; I++ )
			++Iter;

		BListNode *NodeToDelete = Iter.Node->Next;
		Iter.Node->Next = Iter.Node->Next->Next;
		delete NodeToDelete;
	}

	T PopFront()
	{
		ASSERT( HeadList );

		BListNode *Node = HeadList;
		T Result = **Node;
		HeadList = HeadList->Next;
		delete Node;

		Count--;
		
		return Result;
	}

	T PopBack()
	{
		ASSERT( HeadList );

		T Result;

		if( HeadList->Next )
		{
			BListNode* N;

			for( N = HeadList ; N->Next->Next ; N = N->Next );

			Result = **N->Next;
			delete N->Next;
			N->Next = 0;

			Count--;
			
			return Result;
		}
		
		Result = **HeadList;
		delete HeadList;
		HeadList = 0;

		Count--;
		
		return Result;
	}

	T Pop( BIterator &InIter )
	{
		T Result;

		if( Begin() == InIter )
		{
			Result = **HeadList;

			BListNode *Node = HeadList;
			HeadList = HeadList->Next;
			delete Node;

			InIter = End();

			Count--;
			
			return Result;
		}

		for( BIterator Iter = Begin() ; Iter != End() ; ++Iter )
			if( Iter )
				if( Iter.Node->Next == InIter.Node )
				{
					BListNode *Node = Iter.Node->Next;
					Result = **Node;
					Iter.Node->Next = InIter.Node->Next;
					delete Node;

					InIter = End();

					Count--;
					
					return Result;
				}

		SLOW_ASSERT( 0 );

		return T();
	}

	T Pop( const T &InData )
	{
		BIterator Iter = Find( InData );
		return Pop( Iter );
	}

	TList<T,TAllocator> RevertList()const
	{
		TList<T,TAllocator> Result;

		for( BIterator Iter = Begin() ; Iter != End() ; ++Iter )
			Result.InsertFront( *Iter );

		return Result;
	}

	BIterator GetFront()const
	{
		return Begin();
	}

	BIterator GetBack()const
	{
		BIterator Result = Begin();
		
		for( BIterator Iter = Begin() ; Iter != End() ; ++Iter )
			Result = Iter;

		return Result;
	}

	BIterator GetIndex( INT Index )const
	{
		SLOW_ASSERT( Index >= 0 && Index < GetCount() )

		BIterator Iter = Begin();
		for( ; Iter.GetIndex() < Index ; ++Iter );

		return Iter;
	}

	void Free()
	{
		while( HeadList )
			RemoveFront();
		ASSERT( !Count )
	}

	virtual BIterator Find( const T &InData )const
	{
		for( BIterator Iter = Begin() ; Iter != End() ; ++Iter )
			if( *Iter == InData )
				return Iter;

		return End();
	}

	BOOL IsEmpty()const
	{
		return HeadList == 0;
	}

	INT GetCount()const
	{
		return Count;
	}

	BOOL operator==( const TList<T,TAllocator> &InList )const 
	{
		if( InList.GetCount() == GetCount() )
		{
			for( BIterator Iter1 = Begin(), Iter2 = InList.Begin() ; Iter1 != End() ; ++Iter1, ++Iter2 )
				if( *Iter1 != *Iter2)
					return FALSE;

			return TRUE;
		}
		
		return FALSE;
	}

	BOOL operator!=( const TList<T,TAllocator> &InList )const 
	{
		return !( *this == InList );
	}

	TList<T,TAllocator>& operator=( const TList<T,TAllocator> &InList )
	{
		SLOW_ASSERT( &InList != this )
	
		Free();

		// Optimized copy.
		TArray<BListNode*> Nodes( InList.GetCount(), 0 );
		for( BIterator Iter = InList.Begin() ; Iter != InList.End() ; ++Iter )
			Nodes.Add( Iter.Node );

		for( INT F = InList.GetCount()-1 ; F >= 0 ; F-- )
			InsertFront( Nodes[F]->Data );
		
		return *this;
	}

	TList<T,TAllocator>& operator+=( const T &In )
	{
		InsertFront( In );
		return *this;
	}

	TList<T,TAllocator>& operator+=( const TList<T,TAllocator> &InList )
	{
		InsertFront( InList );
		return *this;
	}

	T& operator[]( INT Index )
	{
		return *GetIndex( Index );
	}

	const T& operator[]( INT Index )const
	{
		return *GetIndex( Index );
	}

	friend BOutStream& operator<<<T,TAllocator>( BOutStream &Stream, const TList<T,TAllocator> &In );
	friend BInStream& operator>><T,TAllocator>( BInStream &Stream, TList<T,TAllocator> &In );
};

template<class T, class TAllocator>
BOutStream& operator<<( BOutStream &Stream, const TList<T,TAllocator> &In )
{
	Stream << In.Count;

	for( TList<T,TAllocator>::BIterator Iter = In.Begin() ; Iter != In.End() ; ++Iter )
		Stream << *Iter;

	return Stream;
}

template<class T, class TAllocator>
BInStream& operator>>( BInStream &Stream, TList<T,TAllocator> &In )
{
	INT Count;
	Stream >> Count;

	if( Count )
		for( INT F = 0 ; F < Count ; F++ )
		{
			ASSERT( !Stream.Eos() );

			T Data;
			Stream >> Data;
			In.InsertBack( Data );
		}
	
	return Stream;
}

//=========================//

template<class T, class TAllocator=BFreeListMemoryPool>
struct TVirtualRefList : public TList<T*,TAllocator>
{
private:
	TVirtualRefList( const TVirtualRefList<T,TAllocator> &InList )
	{}

	TVirtualRefList<T,TAllocator>& operator=( const TVirtualRefList<T,TAllocator> &InList )
	{
		return *this;
	}

public:
	TVirtualRefList():
		TList<T*,TAllocator>()
	{}

	virtual ~TVirtualRefList()
	{
		Free();
	}

	virtual void RemoveFront()
	{
		ASSERT( HeadList )

		delete HeadList->Data;
		TList<T*,TAllocator>::RemoveFront();
	}

	virtual void RemoveBack()
	{
		ASSERT(HeadList)

		if( HeadList->Next )
		{
			BListNode *N;

			for( N = HeadList ; N->Next->Next ; N = N->Next );

			delete N->Next->Data;
			delete N->Next;
			N->Next = 0;
		}
		else
		{
			delete HeadList->Data;
			delete HeadList;
			HeadList = 0;
		}

		Count--;
	}

	virtual void Remove( BIterator &InIter )
	{
		if( Begin() == InIter )
		{
			BListNode *Node = HeadList;
			HeadList = HeadList->Next;
			delete Node->Data;
			delete Node;

			InIter = End();
 
			Count--;

			return;
		}

		for( BIterator Iter = Begin() ; Iter != End() ; ++Iter )
			if( Iter )
				if( Iter.Node->Next == InIter.Node )
				{
					BListNode *Node = Iter.Node->Next;
					Iter.Node->Next = InIter.Node->Next;
					delete Node->Data;
					delete Node;

					InIter = End();

					Count--;

					return;
				}

		SLOW_ASSERT( 0 );
	}
};

//=========================//

template<class T, class TAllocator=BFreeListMemoryPool>
struct TRefList : public TVirtualRefList<T,TAllocator>
{
	TRefList():
		TVirtualRefList<T,TAllocator>()
	{}

	TRefList( const TRefList<T,TAllocator> &InList )
	{
		Copy( InList );
	}

	TRefList(INT InSize ):
		TVirtualRefList<T,TAllocator>( InSize )
	{}

	TRefList<T,TAllocator>& operator=( const TRefList<T,TAllocator> &InList )
	{
		Copy( InList );
		return *this;
	}

	virtual void Copy( const TRefList<T,TAllocator> &InList )
	{
		if( &InList != this )
		{
			Free();
			if( InList.Count )				
				for( BIterator Iter = InList.Begin() ; Iter != InList.End() ; ++Iter )
					if( Iter.Node->Data )
						InsertBack( new T(*(Iter.Node->Data)) );
		}
	}
};

//=========================//

template<class T>
class TVirtualAutoPtr
{
protected:
	mutable T	 *Ptr;
	mutable BOOL bMutated;

public:
	TVirtualAutoPtr():
		Ptr( 0 ),
		bMutated( 0 )
	{}

	TVirtualAutoPtr( const TVirtualAutoPtr<T> &InPtr ):
		Ptr( 0 ),
		bMutated( 0 )
	{
		operator=( InPtr );
	}

	TVirtualAutoPtr( T *InPtr ):
		Ptr( InPtr ),
		bMutated( 0 )
	{}

	virtual ~TVirtualAutoPtr()
	{
		Free();
	}

	void MutationWarning()const
	{
		if( bMutated )
			WARN( TEXT("TVirtualAutoPtr"), TEXT("A virtual auto-ptr has mutate.") )
	}

	void Free()
	{
		if( Ptr )
		{
			delete Ptr;
			Ptr = 0;
		}
	}

	TVirtualAutoPtr<T>& operator=( const TVirtualAutoPtr<T> &InPtr )
	{
		Free();
		Ptr				= InPtr.Ptr;
		InPtr.Ptr		= 0;
		InPtr.bMutated	= TRUE;
		return *this;
	}

	T* operator->()const
	{
		MutationWarning();
		return Ptr;
	}

	operator T*()const
	{
		MutationWarning();
		return Ptr;
	}

	T* Get()const
	{
		MutationWarning();
		return Ptr;
	}

	BOOL operator==( const TVirtualAutoPtr<T> &In )const 
	{
		return In.Ptr == Ptr;
	}

	BOOL operator!=( const TVirtualAutoPtr<T> &In )const
	{
		return In.Ptr != Ptr;
	}

	INT Hash()const
	{
		return gHashFunction( (BYTE*)Ptr, sizeof(T*) );
	}

	friend class THashTable<T>;
};

//=========================//

template<class T>
class TAutoPtr : public TVirtualAutoPtr<T>
{
public:
	TAutoPtr():
		TVirtualAutoPtr<T>( 0 )
	{}

	TAutoPtr( T *InPtr ):
		TVirtualAutoPtr<T>( InPtr )
	{}

	TAutoPtr( const TAutoPtr<T> &InPtr )
	{
		*this = InPtr;
	}

	TAutoPtr<T>& operator=( const TAutoPtr<T> &InPtr )
	{
		SLOW_ASSERT( this != &InPtr );

		Free();

		if( InPtr.Ptr )
			Ptr = new T( *InPtr.Ptr );

		return *this;
	}

	TAutoPtr<T>& operator=(T *InPtr)
	{
		Free();
		Ptr = InPtr;
		return *this;
	}

	friend BOutStream& operator<<<T>( BOutStream &Stream, const TAutoPtr<T> &In );
	friend BInStream& operator>><T>( BInStream &Stream, TAutoPtr<T> &In );
	friend class THashTable<T>;
};

template<typename T>
BOutStream& operator<<( BOutStream &Stream, const TAutoPtr<T> &In )
{
	if( In.Ptr )
	{
		Stream << TRUE;
		Stream << *In.Ptr;
	}
	else
		Stream << FALSE;

	return Stream;
}

template<typename T>
BInStream& operator>>( BInStream &Stream, TAutoPtr<T> &In )
{
	BOOL bData;
	Stream >> bData;

	if( bData )
	{
		T *Data = new T;
		Stream >> (T&)*Data;
		In.Ptr = Data;
	}
	
	return Stream;
}

//=========================//

template<class T, class TAllocator=BStdMemoryPool>
class TGraph
{
public:
	class BGraphNode : public BFreeListNode
	{
	protected:
		virtual INT AddChild( BGraphNode *NewNode )
		{
			for( INT F = 0 ; F < Children.GetCount() ; F++ )
				if( Children[F] == 0 )
				{
					Children[F] = NewNode;
					return F;
				}

			return Children.Add( NewNode );
		}

		virtual INT AddChild( const T &InData, INT NbChildren )
		{
			BGraphNode *NewNode = new BGraphNode( InData, NbChildren );
			return AddChild( NewNode );
		}

		virtual INT AddChild( INT NbChildren )
		{
			BGraphNode *NewNode = new BGraphNode;
			NewNode->InitChildren( NbChildren );
			return AddChild( NewNode );
		}

		void DeleteChild( INT Index )
		{
			if( Children[Index] )
				delete Children[Index];
			Children[Index] = 0;
		}

		void InitChildren( INT NbChildren )
		{
			Children.Free();
			if( NbChildren )
			{
				Children.SetCount( NbChildren );
				Children.MemSet( 0 );
			}
		}

	public:
		T Data;
		TArray<BGraphNode*> Children;

		typedef typename TArray<BGraphNode*>::BIterator CChildIterator;

		CChildIterator BeginChild()
		{
			return CChildIterator( Children, 0 );
		}

		CChildIterator EndChild()
		{
			return CChildIterator( Children, Children.GetCount() );
		}

		CChildIterator FindChild( const T &InData )
		{
			for( CChildIterator Iter = BeginChild() ; Iter != EndChild() ; ++Iter )
				if( (*Iter)->Data == InData )
					return Iter;

			return EndChild();
		}

		BGraphNode()
		{}

		BGraphNode( const T &InData ):
			Data( InData )
		{}

		BGraphNode( const T &InData, INT NbChildren ):
			Data( InData )
		{
			if( NbChildren )
			{
				Children.SetCount( NbChildren );
				Children.MemSet( 0 );
			}
		}

		virtual ~BGraphNode()
		{
			Children.Free();
		}

		INT GetChildrenCount()
		{
			INT Result = 0;
			for( INT F = 0 ; F < Children.GetCount() ; F++ )
				if( Children[F] )
					Result++;
			return Result;
		}

		operator T*()
		{
			return &Data;
		}

		T* operator->()
		{
			return &Data;
		}

		VOID* operator new( SIZE_T Size )
		{
			TAllocator *Allocator = GetAllocator();
			BGraphNode *Result = (BGraphNode*)Allocator->New( (INT)Size );
			Result->MemoryPool = Allocator;
			return Result;
		}

		void operator delete( VOID *Ptr )
		{
			BGraphNode *GraphNode = (BGraphNode*)Ptr;
			GraphNode->MemoryPool->Delete( Ptr );
		}

		friend class TGraph<T,TAllocator>;
		friend class TTree<T,TAllocator>;
		friend class TDTree<T,TAllocator>;
	};

	struct BIteratorBase
	{
	protected:
		BGraphNode *Node;

	public:

		BIteratorBase():
		  Node(0)
		{}

		BIteratorBase( const BIteratorBase &Iter )
		{
			*this = Iter;
		}

		BIteratorBase( BGraphNode *InNode ):
			Node( InNode )
		{}

		operator T*()const
		{
			ASSERT( Node );
			return *Node;
		}

		T* operator->()
		{
			ASSERT( Node );
			return *Node;
		}

		BGraphNode* GetNode()const
		{
			return Node;
		}

		friend class TGraph<T,TAllocator>;
		friend class TTree<T,TAllocator>;

		friend typename TGraph<T,TAllocator>::BIterator;
		friend typename TTree<T,TAllocator>::BIterator;
	};

	struct BIterator 
	{
		typename TList<BGraphNode*,TAllocator>::BIterator ListIter;
				
		BIterator()
		{}

		BIterator( const BIterator &InIter )
		{
			*this = InIter;
		}

		BIterator( const typename TList<BGraphNode*,TAllocator>::BIterator &InListIter )
		{
			ListIter = InListIter;
		}

		BIterator( const BIteratorBase &BaseIter )
		{
			*this = Find( BaseIter.Node );
		}

		BIterator& operator++()
		{
			++ListIter;
			return *this;
		}

		BOOL operator==( const BIterator &Iter )const
		{
			return ListIter == Iter.ListIter;
		}

		BOOL operator!=( const BIterator &Iter )const
		{
			return ListIter != Iter.ListIter;
		}

		BGraphNode* GetNode()const
		{
			if( ListIter != TList<BGraphNode*,TAllocator>::End() )
				return *ListIter;
			return 0;
		}

		operator T*()const
		{
			ASSERT( *ListIter );
			return **ListIter;
		}

		T* operator->()
		{
			ASSERT( *ListIter );
			return **ListIter;
		}
	};

protected:
	TGraph<T,TAllocator>( BGraphNode *Node )
	{
		AddNewNodeAndChildrenInList( Node );
	}

	void AddNewNodeAndChildrenInList( BGraphNode *Node )
	{
		ASSERT( Node );

		NodeList.InsertBack( Node );

		for( BGraphNode::CChildIterator ChildIter = Node->BeginChild() ; ChildIter != Node->EndChild() ; ++ChildIter )
			if( *ChildIter )
				AddNewNodeAndChildrenInList( *ChildIter );
	}

	void RemoveNodeAndChildrenInList( BGraphNode *Node )
	{
		for( BGraphNode::CChildIterator ChildIter = Node->BeginChild() ; ChildIter != Node->EndChild() ; ++ChildIter )
			if( *ChildIter )
				RemoveNodeAndChildrenInList( *ChildIter );

		NodeList.Remove( Node );
	}

	void AddNewNodeInList( BGraphNode *Node )
	{
		NodeList.InsertBack( Node );
	}

	void RemoveNodeInList( BGraphNode *Node )
	{
		NodeList.Remove( Node );
	}

	virtual BGraphNode* AddNewNode( const T &InData, BGraphNode *ToNode = 0 )
	{
		if( ToNode )
		{
			INT Index = ToNode->AddChild( InData, NRies );
			AddNewNodeInList( ToNode->Children[ Index ] );
			return ToNode->Children[ Index ];
		}

		if( GetCount() )
		{
			INT Index = (*NodeList.GetFront())->AddChild( InData, NRies );
			AddNewNodeInList( (*NodeList.GetFront())->Children[ Index ] );
			return (*NodeList.GetFront())->Children[ Index ];
		}

		AddNewNodeInList( new TGraph<T,TAllocator>::BGraphNode(InData, NRies) );
		return *NodeList.GetFront();
	}

	virtual BGraphNode* AddNewNode( BGraphNode *ToNode = 0 )
	{
		if( ToNode )
		{
			INT Index = ToNode->AddChild( NRies );
			AddNewNodeInList( ToNode->Children[ Index ] );
			return ToNode->Children[ Index ];
		}

		if( GetCount() )
		{
			INT Index = (*NodeList.GetFront())->AddChild( NRies );
			AddNewNodeInList( (*NodeList.GetFront())->Children[ Index ] );
			return (*NodeList.GetFront())->Children[ Index ];
		}

		BGraphNode *NewNode = new TGraph<T,TAllocator>::BGraphNode();
		NewNode->InitChildren( NRies );
		AddNewNodeInList( NewNode );
		return *NodeList.GetFront();
	}

public:
	TGraph( INT InNRies = 0 ):
	  NRies( InNRies )
	{}

	TGraph( const TGraph<T,TAllocator> &InGraph )
	{
		*this = InGraph;
	}

	virtual ~TGraph()
	{
		Free();
	}

	BGraphNode* GetHeadNode()const
	{
		return NodeList.GetFront() != NodeList.End() ? *NodeList.GetFront() : 0;
	}

	virtual BIteratorBase Add( const T &InData, BGraphNode *ToNode = 0 )
	{
		if( ToNode )
		{
			SLOW_ASSERT( Find(ToNode) );
			BIteratorBase BaseIter = TGraph<T,TAllocator>::Find( InData );
			if( BaseIter.Node )
				return ToNode->Children[ ToNode->AddChild(BaseIter.Node) ];
			return AddNewNode( InData, ToNode );
		}
		
		BIteratorBase BaseIter = TGraph<T,TAllocator>::Find( InData );
		if( BaseIter.Node )
			return (*NodeList.GetFront())->Children[ (*NodeList.GetFront())->AddChild(BaseIter.Node) ];
		return AddNewNode( InData );
	}

	virtual BIteratorBase Add( BGraphNode *ToNode = 0 )
	{
		if( ToNode )
			return AddNewNode( ToNode );
		return AddNewNode();
	}

	virtual BIteratorBase Add( BGraphNode *Node, BGraphNode *ToNode )
	{
		return ToNode->Children[ ToNode->AddChild( Node ) ];
	}

	virtual void DeleteChild( BGraphNode *Node, BGraphNode *Child, BOOL bDeleteNode = TRUE )
	{
		if( Node && Child )
		{
			for( BGraphNode::CChildIterator Iter = Node->BeginChild() ; Iter != Node->EndChild() ; ++Iter )
				if( *Iter == Child )
				{
					Node->Children[Iter] = 0;

					for( BGraphNode::CChildIterator Iter2 = Child->BeginChild() ; Iter2 != Child->EndChild() ; ++Iter2 )
						if( *Iter2 )
							Node->AddChild( *Iter2 );

					if( bDeleteNode )
					{
						NodeList.Remove( Child );
						delete Child;
					}
					break;
				}
		}
	}

	virtual void Delete( BGraphNode *Node )
	{
		if( Node )
		{
			for( BIterator Iter = Begin() ; Iter != End() ; ++Iter )
				for( BGraphNode::CChildIterator ChildIter = (*Iter.ListIter)->BeginChild() ; ChildIter != (*Iter.ListIter)->EndChild() ; ++ChildIter )
					if( *ChildIter == Node )
						DeleteChild( *Iter.ListIter, Node, FALSE );

			NodeList.Remove( Node );
			delete Node;
		}
	}

	virtual void Delete( const T &InData )
	{
		Delete( Find(InData).GetNode() );
	}

	virtual BIterator Find( BGraphNode *InNode )const
	{
		for( BIterator Iter = Begin() ; Iter != End() ; ++Iter )
			if( InNode == Iter.GetNode() )
				return Iter;

		return End();
	}

	virtual BIteratorBase Find( const T &InData )const
	{
		for( BIterator Iter = Begin() ; Iter != End() ; ++Iter )
			if( InData == *Iter )
				return *Iter.ListIter;

		return 0;
	}

	virtual void Free()
	{
		for( BIterator Iter = Begin() ; Iter != End() ; ++Iter )
			delete *Iter.ListIter;
		NodeList.Free();
	}

	TList<BGraphNode*> ListNodes()const
	{
		TList<BGraphNode*> List;

		if( GetHeadNode() )
			ListNodes( List, GetHeadNode());

		return List;
	}

	void ListNodes( TList<BGraphNode*> &InNodeList, BGraphNode *FromNode )const
	{
		ASSERT( FromNode );

		if( InNodeList.Find(FromNode) != InNodeList.End() )
			return;

		InNodeList.InsertBack( FromNode );

		for( INT F = 0 ; F < FromNode->Children.GetCount() ; F++ )
			if( FromNode->Children[F] )
				ListNodes( InNodeList, FromNode->Children[F] );
	}

	BIterator Begin()const
	{
		return BIterator( NodeList.Begin() );
	}

	static BIterator End()
	{
		return BIterator( TList<BGraphNode*,TAllocator>::End() );
	}

	// Not safe, a data will be in the final graph once
	TGraph<T,TAllocator>& operator=( const TGraph<T,TAllocator> &InGraph )
	{
		SLOW_ASSERT( &InGraph != this );

		Free();
		NRies = InGraph.NRies;
		for( BIterator Iter = InGraph.Begin() ; Iter != InGraph.End() ; ++Iter )
		{
			BIteratorBase BaseIter = Find( *Iter );

			if( !BaseIter.Node )
				BaseIter = Add( *Iter );
		
			for( INT F = 0 ; F < (*Iter.ListIter)->Children.GetCount() ; F++ )
				if( (*Iter.ListIter)->Children[F] )
					Add( (*Iter.ListIter)->Children[F]->Data, BaseIter.Node );
		}

		return *this;
	}

	virtual INT GetCount()const
	{
		return NodeList.GetCount();
	}

	INT GetNRies()const
	{
		return NRies;
	}

	void SetNRies(INT NewNRies)
	{
		NRies = NewNRies;
	}

	BOOL operator==( const TGraph<T,TAllocator> &In )const
	{
		return NodeList == In.NodeList;
	}

	BOOL operator!=( const TGraph<T,TAllocator> &In )const
	{
		return !( *this == In );
	}

protected:
	TList<BGraphNode*,TAllocator>	NodeList;
	INT								NRies;

	static TAllocator* GetAllocator()
	{
		static TAllocator *Allocator = new TAllocator;
		return Allocator;
	}

	friend BOutStream& operator<<<T,TAllocator>( BOutStream &Stream, const TGraph<T,TAllocator> &InGraph );
	friend BInStream& operator>><T,TAllocator>( BInStream &Stream, TGraph<T,TAllocator> &InGraph );

	friend BOutStream& operator<< <T,TAllocator>( BOutStream &Stream, const TDTree<T,TAllocator> &InGraph );
	friend BInStream& operator>> <T,TAllocator>( BInStream &Stream, TDTree<T,TAllocator> &InTree );
};

template<class T, class TAllocator>
BOutStream& operator<<( BOutStream &Stream, const TGraph<T,TAllocator> &InGraph )
{
	TList<TGraph<T,TAllocator>::BGraphNode*> NodeList;

	Stream << InGraph.NRies;

	NodeList = InGraph.ListNodes();

	Stream << NodeList.GetCount();
	for( typename TList<TGraph<T,TAllocator>::BGraphNode*>::BIterator Iter = NodeList.Begin() ; Iter != NodeList.End() ; ++Iter )
	{
		Stream << (*Iter)->Data;
		Stream << (*Iter)->Children.GetCount();
		for( typename TGraph<T,TAllocator>::BGraphNode::CChildIterator ChildIter = (*Iter)->BeginChild() ; ChildIter != (*Iter)->EndChild() ; ++ChildIter )
		{
			TList<TGraph<T,TAllocator>::BGraphNode*>::BIterator NodeIter = NodeList.Find( *ChildIter );

			if( NodeIter != NodeList.End() )
				Stream << NodeIter.GetIndex();
			else
				Stream << (INT)-1;
		}
	}

	return Stream;
}

template<class T>
struct TInGraphNode
{
	T			Data;
	INT			ChildrenCount;
	TArray<INT>	ChildrenIndex;

	BOOL operator==( const TInGraphNode<T> &In )const
	{
		return Data == In.Data && ChildrenCount == In.ChildrenCount;
	}

	BOOL operator!=( const TInGraphNode<T> &In )const
	{
		return Data != In.Data || ChildrenCount != In.ChildrenCount;
	}
};

template<class T, class TAllocator>
BInStream& operator>>( BInStream &Stream, TGraph<T,TAllocator> &InGraph )
{
	INT Count;

	InGraph.Free();

	Stream >> InGraph.NRies;
	Stream >> Count;

	if( Count )
	{
		TArray<typename TGraph<T,TAllocator>::BGraphNode*>	GraphNodes;
		TArray<TInGraphNode<T>>								Nodes;
		Nodes.SetCount( Count );
		GraphNodes.Alloc( Count );

		INT F;
		for( F = 0 ; !Stream.Eos() && F < Count ; F++ )
		{
			Stream >> Nodes[F].Data;
			Stream >> Nodes[F].ChildrenCount;
			for( INT I = 0 ; I < Nodes[F].ChildrenCount ; I++ )
			{
				INT Index;
				Stream >> Index;
				Nodes[F].ChildrenIndex.Add( Index );
			}

			typename TGraph<T,TAllocator>::BGraphNode* Node = new typename TGraph<T,TAllocator>::BGraphNode(Nodes[F].Data, Nodes[F].ChildrenCount);
			InGraph.AddNewNodeInList( Node );
			GraphNodes.Add( Node );
		}

		if( Stream.Eos() )
			Count = F;

		for( F = 0 ; F < Count ; F++ )
			for( INT I = 0 ; I < Nodes[F].ChildrenCount ; I++ )
				if( Nodes[F].ChildrenIndex[I] != -1 )
					GraphNodes[F]->Children[I] = GraphNodes[ Nodes[F].ChildrenIndex[I] ];
	}

	return Stream;
}

//=========================//

template<class T, class TAllocator=BStdMemoryPool>
class TTree : public TGraph<T,TAllocator>
{
protected:
	TTree( BGraphNode *Node ):
		 TGraph<T,TAllocator>( Node )
	{}

	// *& is not very elegant but is really practical here
	virtual void Delete( BGraphNode *&Node )
	{
		ASSERT( Node );

		TList<BGraphNode*> Nodes;
		for( INT F = 0 ; F < Node->Children.GetCount() ; F++ )
			if( Node->Children[F] )
				Nodes.InsertFront( Node->Children[F] );

		RemoveNodeAndChildrenInList( Node );
		delete Node;
		Node = 0;

		for( TList<BGraphNode*>::BIterator Iter = Nodes.Begin() ; Iter != Nodes.End() ; ++Iter )
		{
			Add( *Iter, GetHeadNode() );
			TTree<T,TAllocator> DummyTree( *Iter );
		}
	}

	void Copy( const TTree<T,TAllocator> &InTree )
	{
		SLOW_ASSERT( &InTree != this );

		Free();
		NRies = InTree.NRies;

		if( InTree.GetCount() )
			Copy( InTree.GetHeadNode(), new BGraphNode() );
	}

	void Copy( BGraphNode *Node, BGraphNode *ToNode )
	{
		**ToNode = **Node;
		AddNewNodeInList( ToNode );

		ToNode->InitChildren( Node->Children.GetCount() );
		for( INT I = 0 ; I < Node->Children.GetCount() ; I++ )
			if( Node->Children[I] )
			{
				ToNode->Children[I] = new BGraphNode();
				Copy( Node->Children[I], ToNode->Children[I] );
			}
	}

public:
	TTree( INT InNRies = 0 ):
		TGraph<T,TAllocator>( InNRies )
	{}

	TTree( const TTree<T,TAllocator> &InTree )
	{
		Copy( InTree );
	}

	virtual BIteratorBase Add( const T &InData, BGraphNode *ToNode = 0 )
	{
		return AddNewNode( InData, ToNode );
	}

	virtual BIteratorBase Add( BGraphNode *ToNode = 0 )
	{
		return AddNewNode( ToNode );
	}

	virtual BIteratorBase Add( BGraphNode *Node, BGraphNode *ToNode )
	{
		ASSERT( Node );

		BIteratorBase Result = Add( Node->Data, ToNode );
		
		for( INT F = 0 ; F < Node->Children.GetCount() ; F++ )
			if( Node->Children[F] )
				Add( Node->Children[F], ToNode );
		return Result;
	}

	virtual BIteratorBase Add( const TTree<T,TAllocator> &InTree, BGraphNode *ToNode = 0 )
	{
		SLOW_ASSERT( this != &InTree )

		if( ToNode && InTree.GetCount() )
		{
			TTree<T,TAllocator> TempTree( InTree );
			AddNewNodeAndChildrenInList( TempTree.GetHeadNode() );
			INT ChildIndex = ToNode->AddChild( TempTree.GetHeadNode() );
			TempTree.NodeList.Free();
			return ToNode->Children[ ChildIndex ];
		}
		
		*this = InTree;
		return GetHeadNode();
	}

	virtual BOOL IsEmpty()
	{
		return !GetCount();
	}

	class BIterator 
	{
	protected:
		TGraph<T,TAllocator>::BGraphNode			*CurrentNode;
		INT											NbPopedNode;

		TList<INT>									IndexList;
		TList<TGraph<T,TAllocator>::BGraphNode*>	NodeList;

	public:
		BIterator():
		  CurrentNode( 0 ),
		  NbPopedNode( 0 )
		{}

		BIterator( const TGraph<T,TAllocator>::BIterator &InIter ):
			NbPopedNode( 0 ),
			CurrentNode( *InIter.ListIter )
		{}

		BIterator( const BIterator &Iter )
		{
			*this = Iter;
		}

		BIterator( TGraph<T,TAllocator>::BGraphNode *InNode ):
			NbPopedNode( 0 ),
			CurrentNode( InNode )
		{}

		BIterator( const BIteratorBase &BaseIter ):
			NbPopedNode( 0 ),
			CurrentNode( BaseIter.Node )
		{}

		virtual ~BIterator()
		{
			Free();
		}

		void Free()
		{
			IndexList.Free();
			NodeList.Free();
		}

		INT GetNbPopedNode()
		{
			return NbPopedNode;
		}

		BIterator& operator++()
		{
			SLOW_ASSERT( CurrentNode )

			NbPopedNode = 0;

			INT I = 0;
			while( I < CurrentNode->Children.GetCount() && !CurrentNode->Children[I] )
				I++; 

			if( I < CurrentNode->Children.GetCount() )
			{
				IndexList.InsertFront(I);
				NodeList.InsertFront(CurrentNode);
				CurrentNode = CurrentNode->Children[I];
			}
			else
			if( IndexList.GetCount() ) 
			{
				INT I;
				TGraph<T,TAllocator>::BGraphNode *RelativeNode;

				do{
					NbPopedNode++;

					I = IndexList.PopFront() + 1;
					RelativeNode = NodeList.PopFront();

					while( I < RelativeNode->Children.GetCount() && !RelativeNode->Children[I] )
						I++; 
				}while( I >= RelativeNode->Children.GetCount() && IndexList.GetCount() );

				if( I < RelativeNode->Children.GetCount() )
				{
					IndexList.InsertFront( I );
					NodeList.InsertFront( RelativeNode );
					CurrentNode = RelativeNode->Children[I];	
				}
				else
				{
					// The end
					CurrentNode = 0; 
				}
			}
			else
			{
				// The end
				CurrentNode = 0;
			}

			return *this;
		}

		BIterator& operator=( const BIterator &It )
		{
			SLOW_ASSERT( this != &It );
			Free();

			CurrentNode = It.CurrentNode;
			NbPopedNode = It.NbPopedNode;
			IndexList	= It.IndexList;
			NodeList	= It.NodeList;

			return *this;
		}

		BOOL operator==( const BIterator &Iter )const
		{
			return CurrentNode == Iter.CurrentNode;
		}

		BOOL operator!=( const BIterator &Iter )const
		{
			return CurrentNode != Iter.CurrentNode;
		}

		operator T*()const
		{
			ASSERT( CurrentNode )
			return &(CurrentNode->Data);
		}

		T* operator->()const
		{
			ASSERT( CurrentNode )
			return &(CurrentNode->Data);
		}

		BGraphNode* GetNode()const
		{
			return CurrentNode;
		}

		friend class TTree<T,TAllocator>;
	};

	BIterator Begin()const
	{
		return BIterator( GetHeadNode() );
	}

	static BIterator End() 
	{
		return BIterator( 0 );
	}

	TTree<T,TAllocator>& operator=( const TTree<T,TAllocator> &InTree )
	{
		Copy( InTree );
		return *this;
	}

	BOOL operator==( const TTree<T,TAllocator> &InTree )
	{
		if( GetCount() == InTree.GetCount() )
		{
			TTree<T,TAllocator>::BIterator	Iter	= Begin(),
											Iter2	= InTree.Begin();

            for( ; Iter != End() ; ++Iter, ++Iter2 )
				if( *Iter != *Iter2 )
					return FALSE;

			return TRUE;
		}

		return FALSE;
	}
};

//=========================//

template<class T, class TAllocator=BStdMemoryPool>
class TDTree : public TTree<T,TAllocator>
{
protected:
	virtual BGraphNode* AddNewNode( const T &InData, BGraphNode *ToNode = 0 )
	{
		if( ToNode )
		{
			INT Index = ((BTDTreeNode*)ToNode)->AddChild( InData, NRies );
			AddNewNodeInList( ToNode->Children[ Index ] );
			return ToNode->Children[ Index ];
		}

		if( GetCount() )
		{
			INT Index = ((BTDTreeNode*)(*NodeList.GetFront()))->AddChild( InData, NRies );
			AddNewNodeInList( (*NodeList.GetFront())->Children[ Index ] );
			return (*NodeList.GetFront())->Children[ Index ];
		}

		AddNewNodeInList( (BGraphNode*)new BTDTreeNode(InData, NRies) );
		return *NodeList.GetFront();
	}

	virtual BGraphNode* AddNewNode( BGraphNode *ToNode = 0 )
	{
		if( ToNode )
		{
			INT Index = ((BTDTreeNode*)ToNode)->AddChild( NRies );
			AddNewNodeInList( ToNode->Children[ Index ] );
			return ToNode->Children[ Index ];
		}

		if( GetCount() )
		{
			INT Index = ((BTDTreeNode*)(*NodeList.GetFront()))->AddChild( NRies );
			AddNewNodeInList( (*NodeList.GetFront())->Children[ Index ] );
			return (*NodeList.GetFront())->Children[ Index ];
		}

		BTDTreeNode *NewNode = new BTDTreeNode();
		NewNode->InitChildren( NRies );
		AddNewNodeInList( (BGraphNode*)NewNode );
		return *NodeList.GetFront();
	}

public:
	class BTDTreeNode : public BGraphNode
	{
	protected:
		BTDTreeNode *Parent;

	public:
		BTDTreeNode():
			BGraphNode(),
			Parent( 0 )
		{}

		BTDTreeNode( const T &InData ):
			BGraphNode( InData ),
			Parent( 0 )
		{}

		BTDTreeNode( const T &InData, INT NbChildren ):
			BGraphNode( InData, NbChildren ),
			Parent( 0 )
		{}

		BTDTreeNode* GetParent()const
		{
			return Parent;
		}

		virtual INT AddChild( BGraphNode *NewNode )
		{
			((BTDTreeNode*)NewNode)->Parent = this;
			return BGraphNode::AddChild( NewNode );
		}

		virtual INT AddChild( INT NbChildren )
		{
			BGraphNode *NewNode = new BTDTreeNode();
			((BTDTreeNode*)NewNode)->InitChildren( NbChildren );
			((BTDTreeNode*)NewNode)->Parent = this;
			return BGraphNode::AddChild( NewNode );
		}

		virtual INT AddChild( const T &InData, INT NbChildren )
		{
			BGraphNode *NewNode = new BTDTreeNode( InData, NbChildren );
			((BTDTreeNode*)NewNode)->Parent = this;
			return BGraphNode::AddChild( NewNode );
		}

		friend BInStream& operator>><T>( BInStream &Stream, TDTree<T,TAllocator> &InTree );

		VOID* operator new( SIZE_T Size )
		{
			TAllocator *Allocator = GetAllocator();
			BTDTreeNode *Result = (BTDTreeNode*)Allocator->New( (INT)Size );
			Result->MemoryPool = Allocator;
			return Result;
		}

		void operator delete( VOID *Ptr )
		{
			BTDTreeNode *TreeNode = (BTDTreeNode*)Ptr;
			TreeNode->MemoryPool->Delete( Ptr );
		}
	};

	TDTree( INT InNRies = 0 ):
		TTree<T,TAllocator>( InNRies )
	{}

	TDTree( const TDTree<T,TAllocator> &InTree ): 
		TTree<T,TAllocator>( InTree )
	{}

	virtual BIteratorBase Add( const T &InData, BGraphNode *ToNode = 0 )
	{
		return AddNewNode( InData, ToNode );
	}

	virtual BIteratorBase Add( BGraphNode *ToNode = 0 )
	{
		return AddNewNode( ToNode );
	}

	virtual BIteratorBase Add( BGraphNode *Node, BGraphNode *ToNode )
	{
		ASSERT( Node );

		BIteratorBase Result = Add( Node->Data, ToNode );
		
		for( INT F = 0 ; F < Node->Children.GetCount() ; F++ )
			if( Node->Children[F] )
				Add( Node->Children[F], ToNode );
		return Result;
	}

	virtual BIteratorBase Add( const TDTree<T,TAllocator> &InTree, BTDTreeNode *ToNode = 0 )
	{
		SLOW_ASSERT( this != &InTree )

		if( ToNode && InTree.GetCount() )
		{
			TDTree<T,TAllocator> TempTree( InTree );
			AddNewNodeAndChildrenInList( TempTree.GetHeadNode() );
			INT ChildIndex = ToNode->AddChild( TempTree.GetHeadNode() );
			TempTree.NodeList.Free();
			return ToNode->Children[ChildIndex];
		}
		
		*this = InTree;
		return GetHeadNode();
	}

	BTDTreeNode* GetHeadNode()const
	{
		return NodeList.GetFront() != NodeList.End() ? (BTDTreeNode*)*NodeList.GetFront() : 0;
	}
};

template<class T, class TAllocator>
BOutStream& operator<<( BOutStream &Stream, const TDTree<T,TAllocator> &InTree )
{
	TList<TDTree<T,TAllocator>::BGraphNode*> NodeList;

	Stream << InTree.NRies;
	NodeList = InTree.ListNodes();

	Stream << NodeList.GetCount();
	for( typename TList<TDTree<T,TAllocator>::BGraphNode*>::BIterator Iter = NodeList.Begin() ; Iter != NodeList.End() ; ++Iter )
	{
		TDTree<T,TAllocator>::BTDTreeNode *Node = (TDTree<T,TAllocator>::BTDTreeNode*)*Iter;

		Stream << Node->Data;
		Stream << Node->Children.GetCount();
		for( typename TDTree<T,TAllocator>::BTDTreeNode::CChildIterator ChildIter = Node->BeginChild() ; ChildIter != Node->EndChild() ; ++ChildIter )
		{
			TList<TDTree<T,TAllocator>::BGraphNode*>::BIterator NodeIter = NodeList.Find( *ChildIter );

			if( NodeIter != NodeList.End() )
				Stream << NodeIter.GetIndex();
			else
				Stream << (INT)-1;
		}

		TList<TDTree<T,TAllocator>::BGraphNode*>::BIterator NodeIter = NodeList.Find( (TDTree<T,TAllocator>::BGraphNode*)Node->GetParent() );
		if( NodeIter != NodeList.End() )
			Stream << NodeIter.GetIndex();
		else
			Stream << (INT)-1;
	}

	return Stream;
}

template<class T>
struct TInTDTreeNode
{
	T			Data;
	INT			ChildrenCount;
	TArray<INT>	ChildrenIndex;
	INT			ParentIndex;

	BOOL operator==( const TInTDTreeNode<T> &In )const
	{
		return Data == In.Data && ChildrenCount == In.ChildrenCount;
	}

	BOOL operator!=( const TInTDTreeNode<T> &In )const
	{
		return !( *this == In );
	}
};

template<class T, class TAllocator>
BInStream& operator>>( BInStream &Stream, TDTree<T,TAllocator> &InTree )
{
	INT Count;

	InTree.Free();

	Stream >> InTree.NRies;
	Stream >> Count;

	if( Count )
	{
		TArray<typename TDTree<T,TAllocator>::BTDTreeNode*>	GraphNodes;
		TArray<TInTDTreeNode<T>>							Nodes;
		Nodes.SetCount( Count );
		GraphNodes.Alloc( Count );

		INT F;
		for( F = 0 ; !Stream.Eos() && F < Count ; F++ )
		{
			Stream >> Nodes[F].Data;
			Stream >> Nodes[F].ChildrenCount;
			for( INT I = 0 ; !Stream.Eos() && I < Nodes[F].ChildrenCount ; I++ )
			{
				INT Index;
				Stream >> Index;
				Nodes[F].ChildrenIndex.Add( Index );
			}

			INT Index;
			Stream >> Index;
			Nodes[F].ParentIndex = Index;

			typename TDTree<T,TAllocator>::BTDTreeNode* Node = new typename TDTree<T,TAllocator>::BTDTreeNode(Nodes[F].Data, Nodes[F].ChildrenCount);
			InTree.AddNewNodeInList( (typename TDTree<T,TAllocator>::BGraphNode*)Node );
			GraphNodes.Add( Node );
		}

		if( Stream.Eos() )
			Count = F;

		for( F = 0 ; F < Count ; F++ )
		{
			for( INT I = 0 ; I < Nodes[F].ChildrenCount ; I++ )
				if( Nodes[F].ChildrenIndex[I] != -1 )
					GraphNodes[F]->Children[I] = GraphNodes[ Nodes[F].ChildrenIndex[I] ];

			if( Nodes[F].ParentIndex != -1)
				GraphNodes[F]->Parent = GraphNodes[ Nodes[F].ParentIndex ];
		}
	}

	return Stream;
}

//=========================//

template<class T, class TAllocator=BStdMemoryPool>
class TSearchTree : public TTree<T,TAllocator>
{
protected:
	void AddBranch( BGraphNode *Node, BGraphNode *Branch )
	{
		if( Branch )
			if( (T&)*Branch < (T&)*Node )
				if( Node->Children[0] )
					AddBranch( Node->Children[0], Branch );
				else
					Node->Children[0] = Branch;
			else
				if( Node->Children[1] )
					AddBranch( Node->Children[1], Branch );
				else
					Node->Children[1] = Branch;
	}

	void DeleteNode( BGraphNode *Node, BGraphNode *ToNode = 0 )
	{
		if( ToNode )
		{
			if( ToNode->Children[0] == Node )
				if( Node->Children[0] )
					ToNode->Children[0] = Node->Children[0];
				else
					ToNode->Children[0] = Node->Children[1];
			else
				if( Node->Children[0] )
					ToNode->Children[1] = Node->Children[0];
				else
					ToNode->Children[1] = Node->Children[1];

			if( Node->Children[0] && Node->Children[1] )
			{
				AddBranch( Node->Children[1], Node->Children[0]->Children[1] );
				Node->Children[0]->Children[1] = Node->Children[1];		
			}
		}
		else //HeadList
		{
			if( Node->Children[0] && Node->Children[1] )
			{
				TGraph<T,TAllocator>::BIterator Iter=++TGraph<T,TAllocator>::Begin();

				if( Iter.GetNode( )== Node->Children[1] )
				{
					AddBranch( Node->Children[0], Node->Children[1]->Children[0] );
					Node->Children[1]->Children[0]=Node->Children[0];		
				}
				else
				{
					AddBranch( Node->Children[1], Node->Children[0]->Children[1] );
					Node->Children[0]->Children[1] = Node->Children[1];		
				}
			}
		}
				
		RemoveNodeInList( Node );
		delete Node;
	}

public:
	TSearchTree():
	  TTree<T,TAllocator>( SEARCH_TREE_NRIES )
	{}

	TSearchTree( const TSearchTree<T,TAllocator> &InTree ): 
		TTree<T,TAllocator>( InTree )
	{}

	virtual BIteratorBase Add( const T &InData, BGraphNode *ToNode = 0 )
	{
		if( ToNode )
		{
			if( InData < **ToNode )
			{
				if( ToNode->Children[0] )
					return Add( InData, ToNode->Children[0] );

				ToNode->Children[0] = new TGraph<T,TAllocator>::BGraphNode( InData, NRies );
				AddNewNodeInList(ToNode->Children[0]);

				return ToNode->Children[0];
			}

			if( ToNode->Children[1] )
				return Add( InData, ToNode->Children[1] );

			ToNode->Children[1] = new TGraph<T,TAllocator>::BGraphNode( InData, NRies );
			AddNewNodeInList( ToNode->Children[1] );

			return ToNode->Children[1];
		}

		BGraphNode *HeadNode = GetHeadNode();
		if( HeadNode )
		{
			if( InData < **HeadNode )
			{
				if( HeadNode->Children[0] )
					return Add( InData, HeadNode->Children[0] );

				HeadNode->Children[0] = new TGraph<T,TAllocator>::BGraphNode( InData, NRies );
				AddNewNodeInList( HeadNode->Children[0] );

				return HeadNode->Children[0];
			}

			if( HeadNode->Children[1] )
				return Add( InData, HeadNode->Children[1] );

			HeadNode->Children[1] = new TGraph<T,TAllocator>::BGraphNode( InData, NRies );
			AddNewNodeInList( HeadNode->Children[1] );

			return HeadNode->Children[1];
		}

		HeadNode = new TGraph<T,TAllocator>::BGraphNode( InData, NRies );
		AddNewNodeInList( HeadNode );

		return HeadNode;
	}

	virtual void Delete( const T &InData, BGraphNode *ToNode = 0 )
	{
		if( ToNode )
		{
			SLOW_ASSERT( InData != **ToNode);

			if( InData < **ToNode )
			{
				if( ToNode->Children[0] )
					if( InData == **ToNode->Children[0] )
						DeleteNode( ToNode->Children[0], ToNode );
					else
						TSearchTree<T,TAllocator>::Delete( InData, ToNode->Children[0] );
			}
			else
			{
				if( ToNode->Children[1] )
					if(InData == **ToNode->Children[1] )
						DeleteNode( ToNode->Children[1], ToNode );
					else
						TSearchTree<T,TAllocator>::Delete( InData, ToNode->Children[1] );
			}
		}
		else
		{
			BGraphNode *HeadNode = GetHeadNode();
			if( HeadNode )
			{
				if( InData == **HeadNode )
					DeleteNode( HeadNode );
				else
					if( InData < **HeadNode )
					{
						if( HeadNode->Children[0] )
							if( InData == **HeadNode->Children[0] )
								DeleteNode( HeadNode->Children[0], HeadNode );
							else
								TSearchTree<T,TAllocator>::Delete( InData, HeadNode->Children[0] );
					}
					else
					{
						if( HeadNode->Children[1] )
							if( InData == **HeadNode->Children[1] )
								DeleteNode( HeadNode->Children[1], HeadNode );
							else
								TSearchTree<T,TAllocator>::Delete( InData, HeadNode->Children[1] );
					}
			}
		}
	}

	virtual BIteratorBase Find( const T &InData, BGraphNode *ToNode = 0 )const 
	{
		if( ToNode )
		{
			if( InData == **ToNode )
				return ToNode;

			if( InData < **ToNode )
			{
				if( ToNode->Children[0] )
					return Find( InData, ToNode->Children[0] );

				return 0;
			}

			if( ToNode->Children[1] )
				return Find( InData, ToNode->Children[1] );

			return 0;
		}

		BGraphNode *HeadNode = GetHeadNode();
		if( HeadNode )
		{
			if( InData == **HeadNode )
				return HeadNode;

			if( InData < **HeadNode )
			{
				if( HeadNode->Children[0] )
					return Find( InData, HeadNode->Children[0] );

				return 0;
			}

			if( HeadNode->Children[1] )
				return Find( InData, HeadNode->Children[1] );
		}

		return 0;
	}

	TSearchTree<T,TAllocator>& operator=( const TSearchTree<T,TAllocator> &InTree )
	{
		TTree<T,TAllocator>::operator =( InTree );
		return *this; 
	}

	struct BIterator
	{
		TGraph<T,TAllocator>::BGraphNode *CurrentNode;
	
		TList<TGraph<T,TAllocator>::BGraphNode*> NodeList;

		BIterator():
		  CurrentNode( 0 )
		{}

		BIterator( const TGraph<T,TAllocator>::BIterator &InIter ):
			CurrentNode( *InIter.ListIter )
		{}

		BIterator( const BIterator &Iter )
		{
			*this = Iter;
		}

		BIterator( TGraph<T,TAllocator>::BGraphNode *InNode ):
			CurrentNode( InNode )
		{}

		BIterator( const BIteratorBase &BaseIter ):
			CurrentNode( BaseIter.GetNode() )
		{}

		virtual ~BIterator()
		{
			Free();
		}

		void Free()
		{
			NodeList.Free();
		}

		BIterator& operator++()
		{
			SLOW_ASSERT( CurrentNode );
			
			if( CurrentNode->Children[1] )
			{
				CurrentNode = CurrentNode->Children[1];
				while( CurrentNode )
				{
					NodeList.InsertFront( CurrentNode );
					CurrentNode = CurrentNode->Children[0];
				}
			}
			
			CurrentNode = NodeList.GetCount() ? NodeList.PopFront() : 0;
	
			return *this;
		}

		BIterator& operator=( const BIterator &It )
		{
			NodeList = It.NodeList;
			CurrentNode = It.CurrentNode;
			return *this;
		}

		BOOL operator==( const BIterator &Iter )const
		{
			return CurrentNode == Iter.CurrentNode;
		}

		BOOL operator!=( const BIterator &Iter )const
		{
			return !( *this == Iter );
		}

		operator T*()const
		{
			ASSERT( CurrentNode )
			return &(CurrentNode->Data);
		}

		T* operator->()const
		{
			ASSERT( CurrentNode )
			return &(CurrentNode->Data);
		}

		BGraphNode* GetNode()const
		{
			return CurrentNode;
		}
	};

	BIterator Begin()const
	{
		BIterator Iter( GetHeadNode() );

		while( Iter.CurrentNode && Iter.CurrentNode->Children[0] )
		{
			Iter.NodeList.InsertFront( Iter.CurrentNode );
			Iter.CurrentNode = Iter.CurrentNode->Children[0];
		}

		return Iter;
	}

	static BIterator End() 
	{
		return BIterator( 0 );
	}

	const T& GetMin()const
	{
		BIterator Iter = Begin();
		ASSERT( Iter != End() )
		return *Iter;
	}

	const T& GetMax()const
	{
		BIterator Iter( GetHeadNode() );

		while( Iter.CurrentNode && Iter.CurrentNode->Children[1] )
		{
			Iter.NodeList.InsertFront( Iter.CurrentNode );
			Iter.CurrentNode = Iter.CurrentNode->Children[1];
		}

		ASSERT( Iter != End() )
		return *Iter;
	}

	BOOL operator==( const TSearchTree<T,TAllocator> &InTree )const
	{
		if( GetCount() == InTree.GetCount() )
		{
			TSearchTree<T,TAllocator>::BIterator	Iter	= Begin(),
													Iter2	= InTree.Begin();

            for( ; Iter != End() ; ++Iter, ++Iter2 )
				if( *Iter != *Iter2 )
					return FALSE;

			return TRUE;
		}

		return FALSE;
	}

	BOOL operator!=( const TSearchTree<T,TAllocator> &InTree )const
	{
		return !( *this == InTree );
	}
};

//=========================//

template<class KeyType>
struct THashIndexer
{
	static INT GetHashIndex( const KeyType &InKey )
	{
		return InKey.Hash();
	}
};

template<> 
struct THashIndexer<VOID*>
{
	static INT GetHashIndex( VOID *InKey )
	{
		PTR I = (PTR)InKey;
		return gHashFunction( (BYTE*)&I, sizeof(PTR) );
	}
};

template<> 
struct THashIndexer<BYTE>
{
	static INT GetHashIndex( const BYTE &InKey )
	{
		return gHashFunction( (BYTE*)&InKey, sizeof(BYTE) );
	}
};

template<> 
struct THashIndexer<SHORT>
{
	static INT GetHashIndex( const SHORT &InKey )
	{
		return gHashFunction( (BYTE*)&InKey, sizeof(SHORT) );
	}
};

template<> 
struct THashIndexer<INT>
{
	static INT GetHashIndex( const INT &InKey )
	{
		return gHashFunction( (BYTE*)&InKey, sizeof(INT) );
	}
};

template<> 
struct THashIndexer<LONG>
{
	static INT GetHashIndex( const LONG &InKey )
	{
		return gHashFunction( (BYTE*)&InKey, sizeof(LONG) );
	}
};

template<> 
struct THashIndexer<UNSIGNED SHORT>
{
	static INT GetHashIndex( const UNSIGNED SHORT &InKey )
	{
		return gHashFunction( (BYTE*)&InKey, sizeof(UNSIGNED SHORT) );
	}
};

template<> 
struct THashIndexer<UNSIGNED INT>
{
	static INT GetHashIndex( const UNSIGNED INT &InKey )
	{
		return gHashFunction( (BYTE*)&InKey, sizeof(UNSIGNED INT) );
	}
};

template<> 
struct THashIndexer<UNSIGNED LONG>
{
	static INT GetHashIndex( const UNSIGNED LONG &InKey )
	{
		return gHashFunction( (BYTE*)&InKey, sizeof(UNSIGNED LONG) );
	}
};

template<> 
struct THashIndexer<FLOAT>
{
	static INT GetHashIndex( const FLOAT &InKey )
	{
		return gHashFunction( (BYTE*)&InKey, sizeof(FLOAT) );
	}
};

template<> 
struct THashIndexer<DOUBLE>
{
	static INT GetHashIndex( const DOUBLE &InKey )
	{
		return gHashFunction( (BYTE*)&InKey, sizeof(DOUBLE) );
	}
};

//=========================//

template<class T>
class THashTable
{
protected:
	TArray<TRefList<T>> Table;
	INT					Count;

	void Add( T *InData ) 
	{
		INT HashIndex = THashIndexer<T>::GetHashIndex( *InData )&HASH_MASK( Table.GetCount() );
		Table[ HashIndex ].InsertFront( InData );
		Count++;
	}

	void Delete( T *InData )
	{
		INT HashIndex = THashIndexer<T>::GetHashIndex( *InData )&HASH_MASK( Table.GetCount() );
		TRefList<T>::BIterator Iter = FindData( Table[ HashIndex ], *InData );
		Table[ HashIndex ].TList<T*>::Remove( Iter );
		Count--;
	}

	typename TRefList<T>::BIterator FindData( const TRefList<T> &List, const T &InData )const
	{
		for( TRefList<T>::BIterator Iter = List.Begin() ; Iter != List.End() ; ++Iter )
			if( **Iter == InData )
				return Iter;
		return List.End();
	}

public:
	struct BIterator 
	{
		const TArray<TRefList<T>>				*Array;
		typename TArray<TRefList<T>>::BIterator ArrayIter;
		typename TRefList<T>::BIterator			ListIter;

		BIterator()
		{}

		BIterator& operator++()
		{
			++ListIter;
			if( ListIter == TRefList<T>::End() )
			{
				do{
					++ArrayIter;
				}while( ArrayIter != Array->End() && !ArrayIter->GetCount() );
				if( ArrayIter != Array->End() )
					ListIter = ArrayIter->Begin();
			}
			return *this;
		}

		operator T*()const
		{
			return *ListIter;
		}

		T* operator->()const
		{
			return *ListIter;
		}

		BOOL operator==( const BIterator &InIter )const
		{
			return ArrayIter == InIter.ArrayIter && ListIter == InIter.ListIter;
		}

		BOOL operator!=( const BIterator &InIter )const
		{
			return !( *this == InIter );
		}
	};

	THashTable():
		Count( 0 )
	{}

	virtual ~THashTable()
	{}

	void Resize()
	{
		TArray<T*> TmpArray( Table.GetCount(), 0 );
		for( BIterator Iter = Begin() ; Iter != End() ; ++Iter )
		{
			TmpArray.Add( *Iter.ListIter );
			*Iter.ListIter = 0;
		}
		Table.SetCount( Table.GetCount()<<2, TRUE );
		Count = 0;
		for( TArray<T*>::BIterator Iter = TmpArray.Begin() ; Iter != TmpArray.End() ; ++Iter )
			Add( *Iter );
	}

	virtual void Add(const T &InData)
	{
		if( !Table.GetCount() )
			Table.SetCount( 1<<DEFAULT_HASH_TABLE_SIZE );

		INT HashIndex = THashIndexer<T>::GetHashIndex( InData )&HASH_MASK( Table.GetCount() );
		TRefList<T>::BIterator Iter = FindData( Table[ HashIndex ], InData );
		if( Iter != Table[ HashIndex ].End() )
		{
			// For multimaps.
			**Iter = InData;
			return;
		}
		Table[ HashIndex ].InsertFront( new T(InData) );
		Count++;
		if( Table[ HashIndex ].GetCount() > HASH_TABLE_MAX_COLLISION )
			Resize();
	}

	virtual void Add( const THashTable<T> &InTable )
	{
		for( BIterator Iter = InTable.Begin() ; Iter != InTable.End() ; ++Iter )
			Add( *Iter );
	}

	virtual void Delete( const T &InData )
	{
		if( Table.GetCount() )
		{
			INT HashIndex = THashIndexer<T>::GetHashIndex( InData )&HASH_MASK( Table.GetCount() );
			TRefList<T>::BIterator Iter = FindData( Table[ HashIndex ], InData );
			if( Iter != Table[ HashIndex ].End() )
			{
				Table[ HashIndex ].Remove( Iter );
				Count--;
			}
		}
	}

	virtual T* Find( const T &InData )const
	{
		if( Table.GetCount() )
		{
			INT HashIndex = THashIndexer<T>::GetHashIndex( InData )&HASH_MASK( Table.GetCount() );
			TRefList<T>::BIterator Iter = FindData( Table[ HashIndex ], InData );
			if( Iter != Table[ HashIndex ].End() )
				return *Iter;
		}
		return 0;
	}

	virtual void ChangeData( T *PtrData, const T &NewData )
	{
		SLOW_ASSERT( Count )
		Delete( PtrData );
		*PtrData = NewData;
		Add( PtrData );
	}

	const T& operator[]( const T &Index )const
	{
		T *Element = Find( Index );
	
		if( !Element )
			FATAL_ERROR( TEXT("const T& THashTable::operator[](const T &Index)const"), TEXT("Invalid index"));

		return *Element;
	}

	BIterator Begin()const
	{
		BIterator Iter;
		Iter.Array = &Table;
		Iter.ArrayIter = Table.Begin();
		Iter.ListIter = TRefList<T>::End();
		while( Iter.ArrayIter != Iter.Array->End() && !Iter.ArrayIter->GetCount() )
			++Iter.ArrayIter;
		if( Iter.ArrayIter != Iter.Array->End() )
			Iter.ListIter = Iter.ArrayIter->Begin();
		return Iter;
	}

	BIterator End()const
	{
		BIterator Iter;
		Iter.Array = &Table;
		Iter.ArrayIter = Table.End();
		Iter.ListIter = TRefList<T>::End();
		return Iter;
	}

	BOOL IsEmpty()const
	{
		return !Count;
	}

	void Free()
	{
		Table.Free();
		Count = 0;
	}

	INT GetCount()const
	{
		return Count;
	}

	BOOL operator==( const THashTable<T> &InHashTable )const
	{
		return Count == InHashTable.Count && Table == InHashTable.Table;
	}

	BOOL operator!=( const THashTable<T> &Table )const
	{
		return !(*this == Table);
	}

	friend BOutStream& operator<<<T>( BOutStream &Stream, const THashTable<T> &In );
	friend BInStream& operator>><T>( BInStream &Stream, THashTable<T> &In );
};

template<typename T>
BOutStream& operator<<( BOutStream &Stream, const THashTable<T> &In )
{
	TArray<T> TmpArray( In.GetCount(), 0 );
	FOR_EACH( THashTable<T>, Iter, In )
		TmpArray.Add( *Iter );
	Stream << TmpArray;
	return Stream;
}

template<typename T>
BInStream& operator>>( BInStream &Stream, THashTable<T> &In )
{
	TArray<T> TmpArray;
	Stream >> TmpArray;
	FOR_EACH( TArray<T>, Iter, TmpArray )
		In.Add( *Iter );
	return Stream;
}

//=========================//

template<class A, class B>
struct TPair
{
	A First;
	B Second;

	TPair()
	{}

	TPair( const A &InFirst, const B &InSecond ):
		First( InFirst ),
		Second( InSecond )
	{}

	TPair( const A &InFirst ):
		First( InFirst )
	{}

	TPair( const TPair<A,B> &InPair ):
		First( InPair.First ),
		Second( InPair.Second )
	{}

	TPair<A,B>& operator=( const TPair<A,B> &InPair )
	{
		First = InPair.First;
		Second = InPair.Second;

		return *this;
	}

	BOOL operator==( const TPair<A,B> &InPair )const 
	{
		return First == InPair.First;
	}

	BOOL operator!=( const TPair<A,B> &InPair )const
	{
		return First != InPair.First;
	}

	BOOL operator<=( const TPair<A,B> &InPair )const
	{
		return First <= InPair.First;
	}

	BOOL operator>=( const TPair<A,B> &InPair )const
	{
		return First >= InPair.First;
	}

	BOOL operator<( const TPair<A,B> &InPair )const
	{
		return First < InPair.First;
	}

	BOOL operator>( const TPair<A,B> &InPair )const
	{
		return First > InPair.First;
	}

	INT Hash()const
	{
		return THashIndexer<A>::GetHashIndex( First );
	}
};

template<class A, class B>
BOutStream& operator<<( BOutStream &Stream, const TPair<A,B> &In )
{
	Stream << In.First;
	Stream << In.Second;
	return Stream;
}

template<class A, class B>
BInStream& operator>>( BInStream &Stream, TPair<A,B> &In )
{
	Stream >> In.First;
	Stream >> In.Second;
	return Stream;
}

//=========================//

template<class A, class B>
struct TMultiPair : public TPair<A, TList<B>>
{
	TMultiPair()
	{}

	TMultiPair( const A &InFirst, const TList<B> &InSecond ):
		TPair<A,TList<B>>( InFirst, InSecond )
	{}

	TMultiPair( const A &InFirst, const B &InSecond ):
		TPair<A,TList<B>>( InFirst )
	{
		Second.InsertFront( InSecond );
	}

	TMultiPair( const A &InFirst ):
		TPair<A,TList<B>>( InFirst )
	{}

	TMultiPair( const TMultiPair<A,B> &InPair ):
		TPair<A,TList<B>>( InPair.First, InPair.Second )
	{}

	TMultiPair<A,B>& operator=( const TMultiPair<A,B> &InPair )
	{
		First = InPair.First;
		Second.InsertFront( InPair.Second );
		return *this;
	}
};

//=========================//

template<class KeyType, class DataType>
struct TMap : public THashTable<TPair<KeyType,DataType>>
{
	virtual void Add( const TPair<KeyType,DataType> &InData )
	{
		THashTable<TPair<KeyType,DataType>>::Add( InData );
	}

	void Add( const KeyType &Key, const DataType &Data )
	{
		THashTable<TPair<KeyType,DataType>>::Add( TPair<KeyType,DataType>( Key, Data ) );
	}

	void Add(const TMap<KeyType,DataType> &InMap)
	{
		THashTable<TPair<KeyType,DataType>>::Add( InMap );
	}

	DataType& operator[]( const KeyType &Index )
	{
		TPair<KeyType, DataType> *Pair = Find ( Index );
	
		if( !Pair )
			FATAL_ERROR( TEXT("DataType& operator[](const KeyType &Index)"), TEXT("Invalid index") );

		return Pair->Second;
	}

	const DataType& operator[]( const KeyType &Index )const
	{
		TPair<KeyType, DataType> *Pair = Find ( Index );
	
		if( !Pair )
			FATAL_ERROR( TEXT("const DataType& operator[](const KeyType &Index)const"), TEXT("Invalid index"));

		return Pair->Second;
	}
};

//=========================//

template<class KeyType, class DataType>
struct TMultiMap : public THashTable<TMultiPair<KeyType,DataType>>
{
	virtual void Add( const TMultiPair<KeyType,DataType> &InData )
	{
		THashTable<TMultiPair<KeyType,DataType>>::Add( InData );
	}

	void Add( const KeyType &Key, const DataType &Data )
	{
		THashTable<TMultiPair<KeyType,DataType>>::Add( TMultiPair<KeyType,DataType>( Key, Data ) );
	}

	void Add( const TMultiMap<KeyType,DataType> &InMultiMap )
	{
		THashTable<TMultiPair<KeyType,DataType>>::Add( InMultiMap );
	}

	TList<DataType>& operator[]( const KeyType &Index )
	{
		TMultiPair<KeyType, DataType> *MultiPair = Find ( Index );
	
		if( !MultiPair )
			FATAL_ERROR( TEXT("DataType& operator[](const KeyType &Index)"), TEXT("Invalid index") );

		return MultiPair->Second;
	}

	const TList<DataType>& operator[]( const KeyType &Index )const
	{
		TMultiPair<KeyType,DataType> *MultiPair = Find( Index );
	
		if( !MultiPair )
			FATAL_ERROR( TEXT("DataType& operator[](const KeyType &Index)"), TEXT("Invalid index") );

		return MultiPair->Second;
	}
};

//=========================//

template<class Type1, class Type2>
class TBiMap
{
protected:
	TMap<Type1,Type2> Map1;
	TMap<Type2,Type1> Map2;

public:
	struct BIterator 
	{
		typename TMap<Type1,Type2>::BIterator Iter;

		BIterator()
		{}

		BIterator& operator++()
		{
			++Iter;
			return *this;
		}

		operator TPair<Type1,Type2>*()const
		{
			return Iter;
		}

		TPair<Type1,Type2>* operator->()const
		{
			return Iter;
		}

		BOOL operator==( const BIterator &InIter )const
		{
			return Iter == InIter.Iter;
		}

		BOOL operator!=( const BIterator &InIter )const
		{
			return !(*this == InIter);
		}
	};

	virtual ~TBiMap()
	{}

	virtual void Add( const Type1 &Key1, const Type2 &Key2 )
	{
		Add( TPair<Type1,Type2>( Key1, Key2 ) );
	}

	virtual void Add( const TPair<Type1,Type2> &InData )
	{
		Map1.Add( InData );
		Map2.Add( TPair<Type2,Type1>( InData.Second, InData.First ) );
	}

	virtual void Add( const TPair<Type2,Type1> &InData )
	{
		Map1.Add( TPair<Type1,Type2>( InData.Second, InData.First ) );
		Map2.Add( InData );
	}

	// No other versions of this function are possible. Think about it.
	virtual void Delete( const Type1 &Key1, const Type2 &Key2 )
	{
		Map1.Delete( TPair<Type1,Type2>( Key1, Key2 ) );
		Map2.Delete( TPair<Type2,Type1>( Key2, Key1 ) );
	}

	virtual TPair<Type1,Type2>* Find( const Type1 &InData )const
	{
		return Map1.Find( InData );
	}

	virtual TPair<Type2,Type1>* Find( const Type2 &InData )const
	{
		return Map2.Find( InData );
	}

	const Type2& operator[]( const Type1 &Key )const
	{
		return Map1[ Key ];
	}

	Type2& operator[]( const Type1 &Key )
	{
		return Map1[ Key ];
	}

	const Type1& operator[]( const Type2 &Key )const
	{
		return Map2[ Key ];
	}

	Type1& operator[]( const Type2 &Key )
	{
		return Map2[ Key ];
	}

	BIterator Begin()const
	{
		BIterator Iter;
		Iter.Iter = Map1.Begin();
		return Iter;
	}

	BIterator End()const
	{
		BIterator Iter;
		Iter.Iter = Map1.End();
		return Iter;
	}

	void Free()
	{
		Map1.Free();
		Map2.Free();
	}

	INT GetCount()const
	{
		return Map1.GetCount();
	}

	BOOL operator==( const TBiMap<Type1,Type2> &InBiMap )const
	{
		return Map1 == InBiMap.Map1 && Map2 == InBiMap.Map2;
	}

	BOOL operator!=( const TBiMap<Type1,Type2> &InBiMap )const
	{
		return !(*this == Table);
	}
};

//=========================//

template<typename T>
struct TRef
{
	T	Data;
	INT	Count;
	
	TRef():
		Count( 0 )
	{}

	TRef( const T &InData, INT InCount = 0 ):
		Data( InData ),
		Count( InCount )
	{}

	TRef( const TRef<T> &InRef ):
		Data( InRef.Data ),
		Count( InRef.Count )
	{}

	const T& Get()const
	{
		return Data;
	}

	T& Get()
	{
		return Data;
	}

	operator T()const
	{
		return Data;
	}

	operator T&()
	{
		return Data;
	}

	BOOL operator==( const TRef<T> &In )const
	{
		return Data == In.Data;
	}

	BOOL operator!=( const TRef<T> &In )const
	{
		return Data != In.Data;
	}

	BOOL operator<( const TRef<T> &In )const
	{
		return Get() < In.Get();
	}

	BOOL operator<=( const TRef<T> &In )const
	{
		return Get() <= In.Get();
	}

	BOOL operator>( const TRef<T> &In )const
	{
		return Get() > In.Get();
	}

	BOOL operator>=( const TRef<T> &In )const
	{
		return Get() >= In.Get();
	}

	TRef<T>& operator++()
	{
		Count++;
		return *this;
	}

	TRef<T>& operator--()
	{
		Count--;
		return *this;
	}

	INT Hash()const
	{
		return THashIndexer<T>::GetHashIndex( Data );
	}
};

//=========================//

enum EKeyChangeState 
{ 
	EKeyChanged, 
	EKeyAlreadyExist,
	EKeyUserState
};

template<class T>
class TKey 
{
protected:
	TRef<T> *Ptr;

	static THashTable<TRef<T>>& GetRefs()
	{
		static THashTable<TRef<T>> *Refs = new THashTable<TRef<T>>;
		return *Refs;
	}
	
	EKeyChangeState ChangeFor( const T &In )
	{
		TRef<T> *RefPtr = GetRefs().Find( TRef<T>(In) );
		if( !RefPtr )
		{ 
			if( !Ptr )
			{
				GetRefs().Add( TRef<T>(In, 1) );
				Ptr = GetRefs().Find( TRef<T>(In, 1) );
			}
			else
				GetRefs().ChangeData(Ptr , TRef<T>(In, Ptr->Count));
		}
		else
		{
			if( Ptr == RefPtr )
				return EKeyChanged;

			return EKeyAlreadyExist; 
		}

		return EKeyChanged;
	}

public:
	TKey():
	  Ptr( 0 )
	{}

	TKey( const T &In ):
		Ptr( 0 )
	{
		TRef<T> *RefPtr = GetRefs().Find( TRef<T>(In) );
		if( !RefPtr )
		{ 
			GetRefs().Add( TRef<T>(In, 1) );
			Ptr = GetRefs().Find( TRef<T>(In, 1) );
		}
		else
		{
			Ptr = RefPtr;
			++(*Ptr);	
		}
	}

	TKey( const TKey<T> &InKey ):
		Ptr( 0 )
	{
		*this = InKey;
	}

	virtual ~TKey()
	{
		Free();
	}

	void Free()
	{
		if( Ptr )
		{
			--(*Ptr);

			if( !Ptr->Count )
				GetRefs().Delete( *Ptr );

			Ptr = 0;
		}
	}

	TRef<T>* GetPtr()const
	{
		return Ptr;
	}

	const T& Get()const
	{
		ASSERT( Ptr );
		return Ptr->Get();
	}

	T& Get()
	{
		ASSERT( Ptr );
		return Ptr->Get();
	}

	EKeyChangeState Set( const T &In )
	{
		return ChangeFor( In );
	}

	operator T()const
	{
		return Get();
	}

	operator T&()
	{
		return Get();
	}

	TKey<T>& operator=( const TKey<T>& InKey )
	{
		SLOW_ASSERT( this != &InKey );
		Free();

		Ptr = InKey.Ptr;

		if( Ptr )
			++(*Ptr);

		return *this;
	}

	BOOL operator==( const TKey<T> &In )const
	{
		return Ptr == In.Ptr;
	}

	BOOL operator!=( const TKey<T> &In )const
	{
		return Ptr != In.Ptr;
	}

	BOOL operator<( const TKey<T> &In )const
	{
		return Get() < In.Get();
	}

	BOOL operator<=( const TKey<T> &In )const
	{
		return Get()<=In.Get();
	}

	BOOL operator>( const TKey<T> &In )const
	{
		return Get() > In.Get();
	}

	BOOL operator>=( const TKey<T> &In )const
	{
		return Get() >= In.Get();
	}

	INT Hash()const
	{
		return THashIndexer<VOID*>::GetHashIndex( (VOID*)Ptr );
	}

	static BOOL DoesKeyAlreadyExist( const T &Key )
	{
		return (BOOL)GetRefs().Find( TRef<T>(Key) );
	}

	static INT GetTotalRefCount()
	{
		return GetRefs().GetCount();
	}
};

//=========================//

// Differ from TKey by dll export mode.
template<class T>
class TSSKey 
{
protected:
	TRef<T> *Ptr;

	static THashTable<TRef<T>>& GetRefs()
	{
		static THashTable<TRef<T>> *Refs = new THashTable<TRef<T>>;
		return *Refs;
	}
	
	EKeyChangeState ChangeFor( const T &In )
	{
		TRef<T> *RefPtr = GetRefs().Find( TRef<T>(In) );
		if( !RefPtr )
		{ 
			if( !Ptr )
			{
				GetRefs().Add( TRef<T>(In, 1) );
				Ptr = GetRefs().Find( TRef<T>(In, 1) );
			}
			else
				GetRefs().ChangeData(Ptr , TRef<T>(In, Ptr->Count));
		}
		else
		{
			if( Ptr == RefPtr )
				return EKeyChanged;

			return EKeyAlreadyExist; 
		}

		return EKeyChanged;
	}

public:
	TSSKey():
	  Ptr( 0 )
	{}

	TSSKey( const T &In ):
		Ptr( 0 )
	{
		TRef<T> *RefPtr = GetRefs().Find( TRef<T>(In) );
		if( !RefPtr )
		{ 
			GetRefs().Add( TRef<T>(In, 1) );
			Ptr = GetRefs().Find( TRef<T>(In, 1) );
		}
		else
		{
			Ptr = RefPtr;
			++(*Ptr);	
		}
	}

	TSSKey( const TSSKey<T> &InKey ):
		Ptr( 0 )
	{
		*this = InKey;
	}

	virtual ~TSSKey()
	{
		Free();
	}

	void Free()
	{
		if( Ptr )
		{
			--(*Ptr);

			if( !Ptr->Count )
				GetRefs().Delete( *Ptr );

			Ptr = 0;
		}
	}

	TRef<T>* GetPtr()const
	{
		return Ptr;
	}

	const T& Get()const
	{
		ASSERT( Ptr );
		return Ptr->Get();
	}

	T& Get()
	{
		ASSERT( Ptr );
		return Ptr->Get();
	}

	EKeyChangeState Set( const T &In )
	{
		return ChangeFor( In );
	}

	operator T()const
	{
		return Get();
	}

	operator T&()
	{
		return Get();
	}

	TSSKey<T>& operator=( const TSSKey<T>& InKey )
	{
		SLOW_ASSERT( this != &InKey );
		Free();

		Ptr = InKey.Ptr;

		if( Ptr )
			++(*Ptr);

		return *this;
	}

	BOOL operator==( const TSSKey<T> &In )const
	{
		return Ptr == In.Ptr;
	}

	BOOL operator!=( const TSSKey<T> &In )const
	{
		return Ptr != In.Ptr;
	}

	BOOL operator<( const TSSKey<T> &In )const
	{
		return Get() < In.Get();
	}

	BOOL operator<=( const TSSKey<T> &In )const
	{
		return Get()<=In.Get();
	}

	BOOL operator>( const TSSKey<T> &In )const
	{
		return Get() > In.Get();
	}

	BOOL operator>=( const TSSKey<T> &In )const
	{
		return Get() >= In.Get();
	}

	INT Hash()const
	{
		return THashIndexer<VOID*>::GetHashIndex( (VOID*)Ptr );
	}

	static BOOL DoesKeyAlreadyExist( const T &Key )
	{
		return (BOOL)GetRefs().Find( TRef<T>(Key) );
	}

	static INT GetTotalRefCount()
	{
		return GetRefs().GetCount();
	}
};

//=========================//