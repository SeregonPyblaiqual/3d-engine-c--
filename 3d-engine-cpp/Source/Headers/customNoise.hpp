class FastNoiseExtraLite
{
private:
    template <typename T>
    struct Arguments_must_be_floating_point_values;
    int mSeed;
    const float mFrequency = 0.01f;
    const float mOctaves = 3;
    const float mLacunarity = 2.0f;
    const float mGain = 0.5f;
    const float mWeightedStrength = 0.0f;
    const float mPingPongStrength = 2.0f;
    const float mFractalBounding = 1 / 1.75f;

public:
    FastNoiseExtraLite(int seed = 1337)
    {
        mSeed = seed;
    }

    template <>
    struct Arguments_must_be_floating_point_values<float>
    {
    };
    template <>
    struct Arguments_must_be_floating_point_values<double>
    {
    };
    template <>
    struct Arguments_must_be_floating_point_values<long double>
    {
    };

    template <typename FNfloat>
    float GetNoise(FNfloat x, FNfloat y)
    {
        Arguments_must_be_floating_point_values<FNfloat>();
        TransformNoiseCoordinate(x, y);
        return GenFractalFBm(x, y);
    }

    template <typename FNfloat>
    float GetNoise(FNfloat x, FNfloat y, FNfloat z) const
    {
        Arguments_must_be_floating_point_values<FNfloat>();
        TransformNoiseCoordinate(x, y, z);
        return GenFractalFBm(x, y, z);
    }

    template <typename FNfloat>
    float GenFractalFBm(FNfloat x, FNfloat y) const
    {
        int seed = mSeed;
        float sum = 0;
        float amp = mFractalBounding;

        for (int i = 0; i < mOctaves; i++)
        {
            float noise = GenNoiseSingle(seed++, x, y);
            sum += noise * amp;
            amp *= Lerp(1.0f, FastMin(noise + 1, 2) * 0.5f, mWeightedStrength);

            x *= mLacunarity;
            y *= mLacunarity;
            amp *= mGain;
        }

        return sum;
    }

    template <typename FNfloat>
    float GenFractalFBm(FNfloat x, FNfloat y, FNfloat z) const
    {
        int seed = mSeed;
        float sum = 0;
        float amp = mFractalBounding;

        for (int i = 0; i < mOctaves; i++)
        {
            float noise = GenNoiseSingle(seed++, x, y, z);
            sum += noise * amp;
            amp *= Lerp(1.0f, (noise + 1) * 0.5f, mWeightedStrength);

            x *= mLacunarity;
            y *= mLacunarity;
            z *= mLacunarity;
            amp *= mGain;
        }

        return sum;
    }

    template <typename FNfloat>
    float GenNoiseSingle(int seed, FNfloat x, FNfloat y) const
    {
        return SingleSimplex(seed, x, y);
    }

    template <typename FNfloat>
    float GenNoiseSingle(int seed, FNfloat x, FNfloat y, FNfloat z) const
    {
        return SingleOpenSimplex2(seed, x, y, z);
    }

    template <typename FNfloat>
    float SingleSimplex(int seed, FNfloat x, FNfloat y) const
    {
        // 2D OpenSimplex2 case uses the same algorithm as ordinary Simplex.

        const float SQRT3 = 1.7320508075688772935274463415059f;
        const float G2 = (3 - SQRT3) / 6;

        /*
         * --- Skew moved to TransformNoiseCoordinate method ---
         * const FNfloat F2 = 0.5f * (SQRT3 - 1);
         * FNfloat s = (x + y) * F2;
         * x += s; y += s;
         */

        int i = FastFloor(x);
        int j = FastFloor(y);
        float xi = (float)(x - i);
        float yi = (float)(y - j);

        float t = (xi + yi) * G2;
        float x0 = (float)(xi - t);
        float y0 = (float)(yi - t);

        i *= PrimeX;
        j *= PrimeY;

        float n0, n1, n2;

        float a = 0.5f - x0 * x0 - y0 * y0;
        if (a <= 0)
            n0 = 0;
        else
        {
            n0 = (a * a) * (a * a) * GradCoord(seed, i, j, x0, y0);
        }

        float c = (float)(2 * (1 - 2 * G2) * (1 / G2 - 2)) * t + ((float)(-2 * (1 - 2 * G2) * (1 - 2 * G2)) + a);
        if (c <= 0)
            n2 = 0;
        else
        {
            float x2 = x0 + (2 * (float)G2 - 1);
            float y2 = y0 + (2 * (float)G2 - 1);
            n2 = (c * c) * (c * c) * GradCoord(seed, i + PrimeX, j + PrimeY, x2, y2);
        }

        if (y0 > x0)
        {
            float x1 = x0 + (float)G2;
            float y1 = y0 + ((float)G2 - 1);
            float b = 0.5f - x1 * x1 - y1 * y1;
            if (b <= 0)
                n1 = 0;
            else
            {
                n1 = (b * b) * (b * b) * GradCoord(seed, i, j + PrimeY, x1, y1);
            }
        }
        else
        {
            float x1 = x0 + ((float)G2 - 1);
            float y1 = y0 + (float)G2;
            float b = 0.5f - x1 * x1 - y1 * y1;
            if (b <= 0)
                n1 = 0;
            else
            {
                n1 = (b * b) * (b * b) * GradCoord(seed, i + PrimeX, j, x1, y1);
            }
        }

        return (n0 + n1 + n2) * 99.83685446303647f;
    }

    static float FastMin(float a, float b) { return a < b ? a : b; }

    template <typename FNfloat>
    static int FastRound(FNfloat f) { return f >= 0 ? (int)(f + 0.5f) : (int)(f - 0.5f); }

    template <typename FNfloat>
    float SingleOpenSimplex2(int seed, FNfloat x, FNfloat y, FNfloat z) const
    {
        // 3D OpenSimplex2 case uses two offset rotated cube grids.

        /*
         * --- Rotation moved to TransformNoiseCoordinate method ---
         * const FNfloat R3 = (FNfloat)(2.0 / 3.0);
         * FNfloat r = (x + y + z) * R3; // Rotation, not skew
         * x = r - x; y = r - y; z = r - z;
         */

        int i = FastRound(x);
        int j = FastRound(y);
        int k = FastRound(z);
        float x0 = (float)(x - i);
        float y0 = (float)(y - j);
        float z0 = (float)(z - k);

        int xNSign = (int)(-1.0f - x0) | 1;
        int yNSign = (int)(-1.0f - y0) | 1;
        int zNSign = (int)(-1.0f - z0) | 1;

        float ax0 = xNSign * -x0;
        float ay0 = yNSign * -y0;
        float az0 = zNSign * -z0;

        i *= PrimeX;
        j *= PrimeY;
        k *= PrimeZ;

        float value = 0;
        float a = (0.6f - x0 * x0) - (y0 * y0 + z0 * z0);

        for (int l = 0;; l++)
        {
            if (a > 0)
            {
                value += (a * a) * (a * a) * GradCoord(seed, i, j, k, x0, y0, z0);
            }

            float b = a + 1;
            int i1 = i;
            int j1 = j;
            int k1 = k;
            float x1 = x0;
            float y1 = y0;
            float z1 = z0;

            if (ax0 >= ay0 && ax0 >= az0)
            {
                x1 += xNSign;
                b -= xNSign * 2 * x1;
                i1 -= xNSign * PrimeX;
            }
            else if (ay0 > ax0 && ay0 >= az0)
            {
                y1 += yNSign;
                b -= yNSign * 2 * y1;
                j1 -= yNSign * PrimeY;
            }
            else
            {
                z1 += zNSign;
                b -= zNSign * 2 * z1;
                k1 -= zNSign * PrimeZ;
            }

            if (b > 0)
            {
                value += (b * b) * (b * b) * GradCoord(seed, i1, j1, k1, x1, y1, z1);
            }

            if (l == 1)
                break;

            ax0 = 0.5f - ax0;
            ay0 = 0.5f - ay0;
            az0 = 0.5f - az0;

            x0 = xNSign * ax0;
            y0 = yNSign * ay0;
            z0 = zNSign * az0;

            a += (0.75f - ax0) - (ay0 + az0);

            i += (xNSign >> 1) & PrimeX;
            j += (yNSign >> 1) & PrimeY;
            k += (zNSign >> 1) & PrimeZ;

            xNSign = -xNSign;
            yNSign = -yNSign;
            zNSign = -zNSign;

            seed = ~seed;
        }

        return value * 32.69428253173828125f;
    }
};