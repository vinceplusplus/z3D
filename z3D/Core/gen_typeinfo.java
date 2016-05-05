
public class gen_typeinfo
{
	public static void main(String args[])
	{
		int max_bases = 8;

		for(int i=2;i<=max_bases;++i)
		{
			String s="";
			String s2="";
			for(int j=0;j<i;++j)
			{
				s+=",BASECLASSNAME"+j;
				s2+=",BASECLASSNAME"+j+"::type_info_static()";
			}
			System.out.println("#define IMPL_TYPEINFO"+i+"(name,desc,CLASSNAME"+s+")	const TypeInfo* CLASSNAME::_typeinfo=TypeInfo::registerType(name,desc,"+i+s2+");");
		}
	}
}