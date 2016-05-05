

public class next_pow2
{
	public static long np2(long n)
	{
		long x=1;
		while(x<n)
			x<<=1;
		return Math.min(x,((long)1)<<30);
	}
	public static void main(String args[])
	{
		long i;
		for(i=0;i<256;++i)
			System.out.print(","+np2(i<<24));
		System.out.println("");
		for(i=0;i<256;++i)
			System.out.print(","+np2(i<<16));
		System.out.println("");
		for(i=0;i<256;++i)
			System.out.print(","+np2(i<<8));
		System.out.println("");
		for(i=0;i<256;++i)
			System.out.print(","+np2(i<<0));
		System.out.println("");
	}
}