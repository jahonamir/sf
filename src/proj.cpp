#include <iostream>
#include <proj_api.h>

#include "Rcpp.h"

// [[Rcpp::export]]
std::string CPL_proj_version(bool b = false) {
	int v = PJ_VERSION;	
	std::stringstream buffer;
	buffer << v / 100 << "." << (v / 10) % 10 << "." <<  v % 10;
	return buffer.str();
}

// [[Rcpp::export]]
Rcpp::List CPL_proj_is_valid(std::string proj4string) {
	Rcpp::List out(2);
	projPJ pj = pj_init_plus(proj4string.c_str());
	if (pj == NULL) {
		out(0) = Rcpp::LogicalVector::create(false);
		out(1) = Rcpp::CharacterVector::create(pj_strerrno(*pj_get_errno_ref()));
	} else {
		out(0) = Rcpp::LogicalVector::create(true);
		char *def = pj_get_def(pj, 0);
		out(1) = Rcpp::CharacterVector::create(def);
		pj_free(pj);
		free(def);
	}
	return out;
}

extern "C" {
// modified from: rgdal/pkg/src/projectit.cpp

// parts from projects.h, which bite with the R headers:
// should these things ever change in the proj API, then we're in trouble.
struct PJconsts;
struct PJ_LIST {
	char	*id;		/* projection keyword */
	struct PJconsts	*(*proj)(struct PJconsts*);/* projection entry point */
	char 	* const *descr;	/* description text */
};
struct PJ_LIST  *pj_get_list_ref( void );
struct PJ_ELLPS {
	char	*id;	/* ellipse keyword name */
	char	*major;	/* a= value */
	char	*ell;	/* elliptical parameter */
	char	*name;	/* comments */
};
struct PJ_ELLPS *pj_get_ellps_ref( void );
struct PJ_DATUMS {
    char    *id;     /* datum keyword */
    char    *defn;   /* ie. "to_wgs84=..." */
    char    *ellipse_id; /* ie from ellipse table */
    char    *comments; /* EPSG code, etc */
};
struct PJ_DATUMS *pj_get_datums_ref( void ); 
struct PJ_UNITS {
	char	*id;	/* units keyword */
	char	*to_meter;	/* multiply by value to get meters */
	char	*name;	/* comments */
};
struct PJ_UNITS *pj_get_units_ref( void );

} // extern "C"

// [[Rcpp::export]]
Rcpp::List CPL_proj_info(int type) {

	Rcpp::List ret;
	switch (type) {
		case 0: {
			Rcpp::List ans(2);
			ans.attr("names") = Rcpp::CharacterVector::create("name", "description");
    		int n = 0;
        	struct PJ_LIST *lp;
        	for (lp = pj_get_list_ref() ; lp->id ; ++lp)
				n++;
			Rcpp::CharacterVector cv0(n);
			Rcpp::CharacterVector cv1(n);
        	n = 0;
        	for (lp = pj_get_list_ref() ; lp->id ; ++lp) {
				cv0(n) = lp->id;
				cv1(n) = *lp->descr;
            	n++;
			}
			ans(0) = cv0;
			ans(1) = cv1;
			ret = ans;
		} break;
		case 1: {
			Rcpp::List ans(4);
			ans.attr("names") = Rcpp::CharacterVector::create("name", 
				"major", "ell", "description");
    		int n = 0;
        	struct PJ_ELLPS *le;
        	for (le = pj_get_ellps_ref(); le->id ; ++le) 
				n++;
			Rcpp::CharacterVector ans0(n);
			Rcpp::CharacterVector ans1(n);
			Rcpp::CharacterVector ans2(n);
			Rcpp::CharacterVector ans3(n);
        	n = 0;
        	for (le = pj_get_ellps_ref(); le->id ; ++le) {
				ans0(n) = le->id;
				ans1(n) = le->major;
				ans2(n) = le->ell;
				ans3(n) = le->name;
            	n++;
        	}
			ans(0) = ans0;
			ans(1) = ans1;
			ans(2) = ans2;
			ans(3) = ans3;
			ret = ans;
		} break;
		case 2: {
			Rcpp::List ans(4);
			ans.attr("names") = Rcpp::CharacterVector::create("name", "ellipse",
				"definition", "description");
    		int n = 0;
        	struct PJ_DATUMS *ld;
        	for (ld = pj_get_datums_ref(); ld->id ; ++ld) 
				n++;
			Rcpp::CharacterVector ans0(n);
			Rcpp::CharacterVector ans1(n);
			Rcpp::CharacterVector ans2(n);
			Rcpp::CharacterVector ans3(n);
        	n = 0;
        	for (ld = pj_get_datums_ref(); ld->id ; ++ld) {
				ans0(n) = ld->id;
				ans1(n) = ld->ellipse_id;
				ans2(n) = ld->defn;
				ans3(n) = ld->comments;
            	n++;
        	}
			ans(0) = ans0;
			ans(1) = ans1;
			ans(2) = ans2;
			ans(3) = ans3;
			ret = ans;
		} break;
		case 3: {
			Rcpp::List ans(3);
			ans.attr("names") = Rcpp::CharacterVector::create("id", "to_meter",
				"name");
    		int n = 0;
        	struct PJ_UNITS *ld;
        	for (ld = pj_get_units_ref(); ld->id ; ++ld) 
				n++;
			Rcpp::CharacterVector ans0(n);
			Rcpp::CharacterVector ans1(n);
			Rcpp::CharacterVector ans2(n);
        	n = 0;
        	for (ld = pj_get_units_ref(); ld->id ; ++ld) {
				ans0(n) = ld->id;
				ans1(n) = ld->to_meter;
				ans2(n) = ld->name;
            	n++;
        	}
			ans(0) = ans0;
			ans(1) = ans1;
			ans(2) = ans2;
			ret = ans;
		} break;
		default:
			throw std::range_error("unknown type");
		break;
	}
	return ret;
}
