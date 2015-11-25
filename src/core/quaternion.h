/*
 * quat.h
 *
 *  Created on: Jul 16, 2015
 *      Author: nbingham
 */

#include "vector.h"

#ifndef quat_h
#define quat_h

namespace core
{

template <class t>
struct quat
{
	quat()
	{
		a = 0;
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
	}

	quat(t vx, t vy, t vz, t a)
	{
		this->a = a;
		this->v[0] = vx;
		this->v[1] = vy;
		this->v[2] = vz;
	}

	quat(vec<t, 3> v, t a)
	{
		this->a = a;
		this->v = v;
	}

	quat(vec<t, 4> aa)
	{
		this->a = (t)cos(aa[3]/(t)2);
		this->v = (vec<t, 3>)aa * (t)sin(aa[3]/(t)2);

		if (fabs(1.0 - mag(*this)) > 1e-6)
		{
			cout << "Error: " <<  mag((vec<t, 3>)aa) << endl;
		}
	}

	~quat()
	{

	}

	vec<t, 3> v;
	t a;

	quat &operator=(quat c)
	{
		a = c.a;
		v = c.v;
		return *this;
	}

	quat &operator+=(quat c)
	{
		*this = *this + c;
		return *this;
	}

	quat &operator-=(quat c)
	{
		*this = *this - c;
		return *this;
	}

	quat &operator*=(quat c)
	{
		*this = *this * c;
		return *this;
	}

	quat &operator/=(quat c)
	{
		*this = *this / c;
		return *this;
	}

	quat &operator+=(t c)
	{
		*this = *this + c;
		return *this;
	}

	quat &operator-=(t c)
	{
		*this = *this - c;
		return *this;
	}

	quat &operator*=(t c)
	{
		*this = *this * c;
		return *this;
	}

	quat &operator/=(t c)
	{
		*this = *this / c;
		return *this;
	}

	vec<t, 4> axisangle()
	{
		if (a == (t)1 || a == (t)-1)
			return vec<t, 4>((t)0, (t)0, (t)1, (t)0);
		else
		{
			// sin2 x + cos2 x = 1 -> sin x = sqrt(1 - cos2 x)
			double angle = acos(a);
			double isangle = 1.0/sin(angle);
			return vec<t, 4>(v[0]*isangle, v[1]*isangle, v[2]*isangle, 2*angle);
		}
	}

	bool nan()
	{
		return a != a || v.nan();
	}
};

template <class t>
ostream &operator<<(ostream &os, quat<t> q)
{
	os << q.v[0] << "i + " << q.v[1] << "j + " << q.v[2] << "k" << " + " << q.a;
	return os;
}

template <class t>
quat<t> operator-(quat<t> q)
{
	return quat<t>(-q.v, -q.a);
}

template <class t0, class t1>
quat<t0> operator+(quat<t0> q0, quat<t1> q1)
{
	return quat<t0>(q0.v + q1.v, q0.a + q1.a);
}

template <class t0, class t1>
quat<t0> operator-(quat<t0> q0, quat<t1> q1)
{
	return quat<t0>(q0.v - q1.v, q0.a - q1.a);
}

template <class t0, class t1>
quat<t0> operator*(quat<t0> q0, quat<t1> q1)
{
	return quat<t0>(q0.a*q1.v[0] + q0.v[0]*q1.a    + q0.v[1]*q1.v[2] - q0.v[2]*q1.v[1],
					q0.a*q1.v[1] - q0.v[0]*q1.v[2] + q0.v[1]*q1.a    + q0.v[2]*q1.v[0],
					q0.a*q1.v[2] + q0.v[0]*q1.v[1] - q0.v[1]*q1.v[0] + q0.v[2]*q1.a   ,
					q0.a*q1.a    - q0.v[0]*q1.v[0] - q0.v[1]*q1.v[1] - q0.v[2]*q1.v[2]);
}

template <class t0, class t1>
quat<t0> operator/(quat<t0> q0, quat<t1> q1)
{
	t0 m = q1.a*q1.a + mag2(q1.v);
	return quat<t0>(-q0.a*q1.v[0] + q0.v[0]*q1.a    - q0.v[1]*q1.v[2] + q0.v[2]*q1.v[1],
					-q0.a*q1.v[1] + q0.v[0]*q1.v[2] + q0.v[1]*q1.a    - q0.v[2]*q1.v[0],
					-q0.a*q1.v[2] - q0.v[0]*q1.v[1] + q0.v[1]*q1.v[0] + q0.v[2]*q1.a   ,
					 q0.a*q1.a    + q0.v[0]*q1.v[0] + q0.v[1]*q1.v[1] + q0.v[2]*q1.v[2]);
}

template <class t>
quat<t> operator+(quat<t> q, t c)
{
	return quat<t>(q.v, q.a + c);
}

template <class t>
quat<t> operator-(quat<t> q, t c)
{
	return quat<t>(q.v, q.a - c);
}

template <class t>
quat<t> operator*(quat<t> q, t c)
{
	return quat<t>(q.v*c, q.a*c);
}

template <class t>
quat<t> operator/(quat<t> q, t c)
{
	return quat<t>(q.v/c, q.a/c);
}

template <class t>
quat<t> operator+(t c, quat<t> q)
{
	return quat<t>(q.v, c + q.a);
}

template <class t>
quat<t> operator-(t c, quat<t> q)
{
	return quat<t>(-q.v, c - q.a);
}

template <class t>
quat<t> operator*(t c, quat<t> q)
{
	return quat<t>(c*q.v, c*q.a);
}

template <class t>
quat<t> operator/(t c, quat<t> q)
{
	t m = c/(q.a*q.a + mag2(q.v));
	return quat<t>(-m*q.v, m*q.a);
}

template <class t>
quat<t> conj(quat<t> q)
{
	return quat<t>(-q.v, q.a);
}

template <class t>
t mag(quat<t> q)
{
	return sqrt(q.a*q.a + mag2(q.v));
}

template <class t>
quat<t> sqrt(quat<t> q)
{
	t N = sqrt(q + mag(q));
	t s2 = 1.41421356237;

	return quat<t>(q.v/(s2*N), N/s2);
}

template <class t>
quat<t> norm(quat<t> q)
{
	return q/mag(q);
}

template <class t>
t dot(quat<t> q0, quat<t> q1)
{
	return q0.a*q1.a + dot(q0.v, q1.v);
}

template <class t>
quat<t> log(quat<t> q)
{
	double omega = acos(q.a);
	double somega = sin(omega);

	if (somega > 0.0)
		return quat<t>(q.v*(t)(omega/somega), 0);
	else
		return quat<t>(vec<t, 3>(0, 0, 0), 0);
}

template <class t>
quat<t> exp(quat<t> q)
{
	t omega = mag(q);
	double somega = sin(omega);
	double comega = cos(omega);

	if (omega > 0)
		return quat<t>((t)(somega/omega)*q.v, comega);
	else
		return quat<t>(vec<t, 3>(0, 0, 0), comega);
}

template <class t>
quat<t> rotate_quat(quat<t> q0, quat<t> q1)
{
	return q1 * q0 * conj(q1);
}

template <class t, class t2>
quat<t> lerp(quat<t> q0, quat<t> q1, t2 p)
{
	return q0*(t)((t2)1 - p) + q1*(t)p;
}

template <class t, class t2>
quat<t> slerp(quat<t> q0, quat<t> q1, t2 p)
{
	double omega = (double)dot(q0, q1);

	if (omega > 0.95 || omega < -0.95)
		return lerp(q0, q1, p);
	else
	{
		omega = acos(omega);
		double somega = sin(omega);
		quat<t> result = (q0*(t)sin(omega - p*omega) + q1*(t)sin(p*omega))/(t)somega;

		if (result.nan() && !q0.nan() && !q1.nan())
		{
			cout << "slerp(" << q0 << ", " << q1 << ", " << p << ")" << endl;
			cout << "dot(q0, q1) = " << dot(q0, q1) << endl;
			cout << "omega = " << omega << endl;
			cout << "somega = " << somega << endl;
			cout << "omega - p*omega = " << omega - p*omega << endl;
			cout << "sin(omega - p*omega) = " << sin(omega - p*omega) << endl;
			cout << "sin(p*omega) = " << sin(p*omega) << endl;
			cout << "q0*(t)sin(omega - p*omega) = " << q0*(t)sin(omega - p*omega) << endl;
			cout << "q1*(t)sin(p*omega) = " << q1*(t)sin(p*omega) << endl;
			cout << "result = " << result << endl;
		}

		return result;
	}
}

template <class t, class t2>
quat<t> squad(quat<t> q0, quat<t> q1, quat<t> q2, quat<t> q3, t2 p)
{
	quat<t> q12 = conj(q1)*q2;
	quat<t> q10 = conj(q1)*q0;
	quat<t> q23 = conj(q2)*q3;
	quat<t> q21 = conj(q2)*q1;

	if (fabs(1.0 - (double)q12.a) < 1e-3 || fabs(1.0 - (double)q21.a) < 1e-3)
		return q1;
	if (fabs(1.0 - (double)q10.a) < 1e-3 || fabs(1.0 - (double)q23.a) < 1e-3)
		return slerp(q1, q2, p);

	quat<t> S0, S1;
	S0 = q1*exp((log(q12) + log(q10))/-4.0);
	S1 = q2*exp((log(q23) + log(q21))/-4.0);
	quat<t> result = slerp(slerp(q1, q2, p), slerp(S0, S1, p), p*((t2)1-p)*(t2)2);

	if (result.nan() && !q0.nan() && !q1.nan() && !q2.nan() && !q3.nan())
	{
		cout << "squad(" << q0 << ", " << q1 << ", " << q2 << ", " << q3 << ", " << p << ")" << endl;
		cout << mag(q0) << " " << mag(q1) << " " << mag(q2) << " " << mag(q3) << endl;
		cout << "conj(q1) = " << conj(q1) << endl;
		cout << "conj(q2) = " << conj(q2) << endl;
		cout << "conj(q1)*q2 = " << conj(q1)*q2 << endl;
		cout << "conj(q1)*q0 = " << conj(q1)*q0 << endl;
		cout << "conj(q2)*q3 = " << conj(q2)*q3 << endl;
		cout << "conj(q2)*q1 = " << conj(q2)*q1 << endl;
		cout << "log(conj(q1)*q2) = " << log(conj(q1)*q2) << endl;
		cout << "log(conj(q1)*q0) = " << log(conj(q1)*q0) << endl;
		cout << "log(conj(q2)*q3) = " << log(conj(q2)*q3) << endl;
		cout << "log(conj(q2)*q1) = " << log(conj(q2)*q1) << endl;
		cout << "-(log(conj(q1)*q2) + log(conj(q1)*q0))/4.0f = " << -(log(conj(q1)*q2) + log(conj(q1)*q0))/4.0 << endl;
		cout << "-(log(conj(q2)*q3) + log(conj(q2)*q1))/4.0f = " << -(log(conj(q2)*q3) + log(conj(q2)*q1))/4.0 << endl;
		cout << "exp(-(log(conj(q1)*q2) + log(conj(q1)*q0))/4.0f) = " << exp(-(log(conj(q1)*q2) + log(conj(q1)*q0))/4.0) << endl;
		cout << "exp(-(log(conj(q2)*q3) + log(conj(q2)*q1))/4.0f) = " << exp(-(log(conj(q2)*q3) + log(conj(q2)*q1))/4.0) << endl;
		cout << "S0 = " << S0 << endl;
		cout << "S1 = " << S1 << endl;
		cout << "slerp(q1, q2, p) = " << slerp(q1, q2, p) << endl;
		cout << "slerp(S0, S1, p) = " << slerp(S0, S1, p) << endl;
		cout << "p*(1-p)*2 = " << p*(1-p)*2 << endl;
		cout << "result = " << result << endl;
	}

	return result;
}

template <class t>
vec<t, 4> rational_map(quat<t> q)
{
	double m = ::sqrt(2.0*(1.0-(double)q.a));
	return vec<t, 4>(q.v[0], q.v[1], q.v[2], (t)1-q.a)/(t)m;
}

template <class t>
quat<t> rational_rmap(vec<t, 4> v)
{
	vec<t, 3> axis = v;
	t amag2 = mag2(axis);
	return quat<t>(2*v[3]*axis, amag2 - v[3]*v[3])/(amag2 + v[3]*v[3]);
}

}

#endif
