/*******************************************************************************
 *
 * @brief	Float to Qn conversion macro 
 * 
 * @author		Sho Ikeda (JJ1LFO@jarl.com)
 * @copyright	2021 Sho Ikeda.
 *
 *	MIT License
 *
 *	Copyright (c) 2021 Sho Ikeda.
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in all
 *	copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 *
 ******************************************************************************/
#ifndef	_F2Q_H
#define	_F2Q_H


#define	F2Qn(fval, qn)		((long long)(1 + fval*(0x02LL<<qn))>>1)		// 2's complement rounding

#define	F2Q15(fval)		F2Qn(fval, 15)
#define	F2Q14(fval)		F2Qn(fval, 14)
#define	F2Q13(fval)		F2Qn(fval, 13)
#define	F2Q12(fval)		F2Qn(fval, 12)
#define	F2Q11(fval)		F2Qn(fval, 11)
#define	F2Q10(fval)		F2Qn(fval, 10)
#define	F2Q9(fval)		F2Qn(fval,  9)
#define	F2Q8(fval)		F2Qn(fval,  8)
#define	F2Q7(fval)		F2Qn(fval,  7)
#define	F2Q6(fval)		F2Qn(fval,  6)
#define	F2Q5(fval)		F2Qn(fval,  5)
#define	F2Q4(fval)		F2Qn(fval,  4)
#define	F2Q3(fval)		F2Qn(fval,  3)
#define	F2Q2(fval)		F2Qn(fval,  2)
#define	F2Q1(fval)		F2Qn(fval,  1)
#define	F2Q0(fval)		F2Qn(fval,  0)

#define F2Q31(fval)		F2Qn(fval, 31)
#define	F2Q30(fval)		F2Qn(fval, 30)	
#define	F2Q29(fval)		F2Qn(fval, 29)	
#define	F2Q28(fval)		F2Qn(fval, 28)	
#define	F2Q27(fval)		F2Qn(fval, 27)	
#define	F2Q26(fval)		F2Qn(fval, 26)	
#define	F2Q25(fval)		F2Qn(fval, 25)	
#define	F2Q24(fval)		F2Qn(fval, 24)	
#define	F2Q23(fval)		F2Qn(fval, 23)	
#define	F2Q22(fval)		F2Qn(fval, 22)	
#define	F2Q21(fval)		F2Qn(fval, 21)	
#define	F2Q20(fval)		F2Qn(fval, 20)	
#define	F2Q19(fval)		F2Qn(fval, 19)	
#define	F2Q18(fval)		F2Qn(fval, 18)	
#define	F2Q17(fval)		F2Qn(fval, 17)	
#define	F2Q16(fval)		F2Qn(fval, 16)	

#define Qn2Q15_SHIFT(qn)	(15 - qn)
#define Qn2Q31_SHIFT(qn)	(31 - qn)

#endif	/* _F2Q_H */
/*******************************************************************************
 *	End
 ******************************************************************************/
