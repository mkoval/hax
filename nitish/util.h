#ifndef UTIL_NITISH_H_
#define UTIL_NITISH_H_

#define ABS(_x_)      (((_x_) > 0) ? (_x_) : -(_x_))
#define MIN(_x_, _y_) (((_x_) < (_y_)) ? (_x_) : (_y_))
#define MAX(_x_, _y_) (((_x_) > (_y_)) ? (_x_) : (_y_))
#define PROP(_mout_, _merr_, _err_) ((_mout_)*MIN((_merr_), (_merr_))/(_merr_))

#endif
