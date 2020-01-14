#include <stan/math/rev.hpp>
#include <boost/math/differentiation/finite_difference.hpp>
#include <boost/math/special_functions/digamma.hpp>
#include <test/unit/math/expect_near_rel.hpp>
#include <gtest/gtest.h>
#include <vector>
#include <algorithm>

namespace neg_binomial_2_test_internal {
struct TestValue {
  int n;
  double mu;
  double phi;
  double value;
  double grad_mu;
  double grad_phi;
};

// Test data generated in Mathematica (Wolfram Cloud). The code can be re-ran at
// https://www.wolframcloud.com/obj/martin.modrak/Published/neg_binomial_2_lpmf.nb
// but is also presented below for convenience:
//
// nb2[n_,mu_,phi_]:= LogGamma[n + phi] - LogGamma[n + 1] - LogGamma[phi] +
//    n * (Log[mu] - Log[mu + phi]) + phi * (Log[phi] - Log[mu + phi])
// nb2dmu[n_,mu_,phi_]= D[nb2[n, mu, phi],mu];
// nb2dphi[n_,mu_,phi_]= D[nb2[n, mu, phi],phi];
// out = OpenWrite["nb_test.txt"]
// mus= {256*10^-7,314*10^-3,15*10^-1,3,180,  1123,10586};
// phis=  {4*10^-4,65*10^-3,442*10^-2,800, 15324};
// ns = {0,6,14,1525,10233};
// WriteString[out, "std::vector<TestValue> testValues = {"];
//   For[k = 1, k <= Length[ns], k++, {
//     For[i = 1, i <= Length[mus], i++, {
//       For[j = 1, j <= Length[phis], j++, {
//         cmu = mus[[i]];
//         cphi = phis[[j]];
//         cn=ns[[k]];
//         val = N[nb2[cn,cmu,cphi], 24];
//         ddmu= N[nb2dmu[cn,cmu,cphi], 24];
//         ddphi= N[nb2dphi[cn,cmu,cphi], 24];
//         WriteString[out," {",CForm[cn],",",CForm[cmu],",",
//           CForm[cphi],",", CForm[val],",",CForm[ddmu],",",CForm[ddphi],"},"]
//       }]
//     }]
//   }]
// WriteString[out,"};"];
// Close[out];
// FilePrint[%]
//
std::vector<TestValue> testValues = {
    {0, 0.0000256, 0.0004, -0.0000248141563677810565248409,
     -0.93984962406015037593985, -0.0018850149796030172519518},
    {0, 0.0000256, 0.065, -0.0000255949600924861403888716,
     -0.999606308899879432100588, -7.75166869111834937935475e-8},
    {0, 0.0000256, 4.42, -0.0000255999258645396480948713,
     -0.999994208178341772500141, -1.67726638232395628338958e-11},
    {0, 0.0000256, 800, -0.0000255999995904000087381331,
     -0.999999968000001023999967, -5.1199997815466745309864e-16},
    {0, 0.0000256, 15324, -0.0000255999999786165492276784,
     -0.999999998329417909342659, -1.39542226236663832355842e-18},
    {0, 0.314, 0.0004, -0.00266678271697168261798476,
     -0.00127226463104325699745547, -5.66822905706024980195936},
    {0, 0.314, 0.065, -0.114604680787141300415594, -0.171503957783641160949868,
     -0.934652892970430398112858},
    {0, 0.314, 4.42, -0.303348202309659616314882, -0.933671313899450781580059,
     -0.00230212890163621513546237},
    {0, 0.314, 800, -0.313938393619808990999524, -0.999607653995806645891488,
     -7.69878314071302369967267e-8},
    {0, 0.314, 15324, -0.313996782998787812397665, -0.999979509686371605280341,
     -2.09929343978617830724328e-10},
    {0, 1.5, 0.0004, -0.00329191110003275493362122,
     -0.00026659557451346307651293, -7.23004434565640079712956},
    {0, 1.5, 0.065, -0.206781499150110071240643, -0.0415335463258785942492013,
     -2.22278737940449507487448},
    {0, 1.5, 4.42, -1.29150964740387939892012, -0.746621621621621621621622,
     -0.0388183744279291779383916},
    {0, 1.5, 800, -1.49859550534427827334156, -0.998128509045539613225203,
     -1.75342721996106687969703e-6},
    {0, 1.5, 15324, -1.49992659053829698010225, -0.999902123911128511304688,
     -4.79017695146261715054393e-9},
    {0, 3, 0.0004, -0.00356911664958785451901203, -0.00013331555792560991867751,
     -7.92292493952756190744874},
    {0, 3, 0.065, -0.250472012600853853120106, -0.0212071778140293637846656,
     -2.87462275628870402717091},
    {0, 3, 4.42, -2.28973397601639586742486, -0.595687331536388140161725,
     -0.113726692626070463108525},
    {0, 3, 800, -2.99438902306750149309853, -0.996264009962640099626401,
     -6.99624147447649277415792e-6},
    {0, 3, 15324, -2.99970638131217673536622, -0.999804266979839498923468,
     -1.91582075605741709616675e-8},
    {0, 180, 0.0004, -0.00520680203358650223341638,
     -2.22221728396159119646401e-6, -12.0170073061835395451321},
    {0, 180, 0.065, -0.515234583883688262086666, -0.000360980756948879571265932,
     -6.92704688665984521936613},
    {0, 180, 4.42, -16.4913562250745368347413, -0.0239670317752955210931569,
     -2.75504310079668394524277},
    {0, 180, 800, -162.3526751973522458866, -0.816326530612244897959184,
     -0.0192673746089352053174335},
    {0, 180, 15324, -178.951041022560568926078, -0.988390092879256965944272,
     -0.0000679211892648339243088643},
    {0, 1123, 0.0004, -0.00593912212871338076639709,
     -3.56188653183026470872139e-7, -13.8478056779721050990192},
    {0, 1123, 0.065, -0.634217014783734204352692,
     -0.000057877326779839101031552, -8.75724272015345990606553},
    {0, 1123, 4.42, -24.4936395934523258117904, -0.00392045555338738003583403,
     -4.54546787488649276731873},
    {0, 1123, 800, -701.624014336681947952778, -0.416016640665626625065003,
     -0.293046658586479060005975},
    {0, 1123, 15324, -1083.75715139725783359741, -0.931720070529579862588922,
     -0.00244293345030929573936621},
    {0, 10586, 0.0004, -0.00683653348042399161482707,
     -3.77857533426883301459161e-8, -16.091333738845732379756},
    {0, 10586, 0.065, -0.780043017108597589318463,
     -6.14014744855619155937546e-6, -11.0006679418181806995525},
    {0, 10586, 4.42, -34.3945190758502000304634, -0.000417358329509122395523501,
     -6.78198276010557247770399},
    {0, 10586, 800, -2124.42246547447991619038, -0.0702617249253469172668189,
     -1.72578980676844681250479},
    {0, 10586, 15324, -8048.29915678058041924358, -0.591431879583172520262447,
     -0.116640647318788574768032},
    {6, 0.0000256, 0.0004, -26.4803625972224902645113,
     220276.315789473684210526, -11595.4634978387095432955},
    {6, 0.0000256, 0.065, -51.4193881364481026728232, 234281.729042100342019143,
     -74.6938075459730303944346},
    {6, 0.0000256, 4.42, -67.5203813751119077704357, 234372.642547590674587948,
     -0.431255924758490025034736},
    {6, 0.0000256, 800, -69.998079065742703551293, 234373.992500032239998968,
     -0.0000233303844710378174909984},
    {6, 0.0000256, 15324, -70.0158073212983954315096, 234373.999608458993084276,
     -6.38614276575077488493398e-8},
    {6, 0.314, 0.0004, -9.62519749441237582251687, 0.023038524497171844864751,
     2477.5305495558165940557},
    {6, 0.314, 0.065, -5.62316274126580027012417, 3.10564173234962943044888,
     0.811756664687528071010215},
    {6, 0.314, 4.42, -11.7481186360489883350416, 16.9071818179371883568924,
     -0.343526975436714222156802},
    {6, 0.314, 800, -13.8270107248858320162615, 18.1011755433762948679586,
     -0.0000204650172704751119362432},
    {6, 0.314, 15324, -13.8425659569858167417764, 18.1079092104353788140892,
     -5.6049178830042368725488e-8},
    {6, 1.5, 0.0004, -9.61978396161923322623754, 0.00079978672354038922953879,
     2491.05377011516735187466},
    {6, 1.5, 0.065, -4.84100453988744409858109, 0.124600638977635782747604,
     11.5208909282007772435409},
    {6, 1.5, 4.42, -4.69471130087565245107321, 2.23986486486486486486486,
     -0.126129611536089773251445},
    {6, 1.5, 800, -5.6375883578559464150792, 2.99438552713661883967561,
     -0.0000110478695320263035818071},
    {6, 1.5, 15324, -5.64599569956598171509704, 2.99970637173338553391406,
     -3.03295927479829163411156e-8},
    {6, 3, 0.0004, -9.61926132713561025018161, 0.00013331555792560991867751,
     2492.36008977011398813188},
    {6, 3, 0.065, -4.75878244235486512052296, 0.0212071778140293637846656,
     12.745335721641112641817},
    {6, 3, 4.42, -2.88910819582874566285473, 0.595687331536388140161725,
     0.00385024685205873541538553},
    {6, 3, 800, -2.98571724360242195248096, 0.996264009962640099626401,
     -2.3069406647953774852549e-6},
    {6, 3, 15324, -2.98747963477610734281426, 0.999804266979839498923468,
     -6.38245840446628643263247e-9},
    {6, 180, 0.0004, -9.62011239916672050497847, -2.14814337449620482324854e-6,
     2490.23240751308273540641},
    {6, 180, 0.065, -4.89709960487442547895843, -0.000348948065050583585557067,
     10.6171759350004776847048},
    {6, 180, 4.42, -11.8028833191445191284052, -0.023168130716119003723385,
     -1.86137525666548784300587},
    {6, 180, 800, -138.973123190861697819951, -0.789115646258503401360544,
     -0.0179131542129975601896404},
    {6, 180, 15324, -154.441639359481734262519, -0.955443756449948400412797,
     -0.0000634392772194434652186986},
    {6, 1123, 0.0004, -9.62083352307562855236427, -3.5428559715533443273747e-7,
     2488.4295995707558483234},
    {6, 1123, 0.065, -5.0142630342277769583843, -0.0000575680979635621334392196,
     8.81495887967631288756553},
    {6, 1123, 4.42, -19.6831821663032130583962, -0.00389950921917515894926679,
     -3.62458748384772905278683},
    {6, 1123, 800, -671.304184750646975825767, -0.41379393376981739999787,
     -0.288690114015941777831476},
    {6, 1123, 15324, -1048.61720731875393149901, -0.926742047000481483982035,
     -0.00241626280594218362665218},
    {6, 10586, 0.0004, -9.62172902400956379331308,
     -3.77643368945439762841293e-8, 2486.19084755337982611478},
    {6, 10586, 0.065, -5.15977860354001470537965, -6.13666729697000818989159e-6,
     6.57630939764217205495288},
    {6, 10586, 4.42, -29.5629973572818969888733, -0.000417121776516768840415515,
     -5.85634703346878397904497},
    {6, 10586, 800, -2091.3124520859014907288, -0.0702219015407302460497774,
     -1.71884010032985743252181},
    {6, 10586, 15324, -8002.42495594488889877697, -0.59109666408369216553719,
     -0.116480739324794834878369},
    {14, 0.0000256, 0.0004, -49.8145624383586400724923,
     513979.323308270676691729, -30391.5592215308067657195},
    {14, 0.0000256, 0.065, -114.928171090288372551513,
     546658.700573312664550577, -196.832392093265929097095},
    {14, 0.0000256, 4.42, -161.649312877052269163198, 546870.832603322478494242,
     -1.65012869090202521315342},
    {14, 0.0000256, 800, -173.098986627797206761932, 546873.982500032559998958,
     -0.00014060727924520899116967},
    {14, 0.0000256, 15324, -173.206165042629407096483,
     546873.999086402089753857, -3.87293689171005773693536e-7},
    {14, 0.314, 0.0004, -10.48232124038756783909, 0.0554529100014586473476929,
     2452.9820144458617504796},
    {14, 0.314, 0.065, -7.91753972042693519284807, 7.47516931919399021898056,
     -19.4065385144529680284843},
    {14, 0.314, 4.42, -31.1096030160603970450565, 40.6949859937193738748557,
     -1.44235830397139693802194},
    {14, 0.314, 800, -41.6146101076375483922037, 43.568886473205763553092,
     -0.000133818772002702504530118},
    {14, 0.314, 15324, -41.7166402819016445593073, 43.585094170597712706582,
     -3.68785202783475625980072e-7},
    {14, 1.5, 0.0004, -10.4688561593594870180174, 0.00222162978761219230427442,
     2486.61861613580838671742},
    {14, 1.5, 0.065, -5.96960149272681243148081, 0.346112886048988285410011,
     7.29895408232336627643618},
    {14, 1.5, 4.42, -13.3341039738636536056565, 6.22184684684684684684685,
     -0.886409460834882606838869},
    {14, 1.5, 800, -20.9264151022685229463224, 8.31773757537949677687669,
     -0.000109610174761583369512834},
    {14, 1.5, 15324, -21.0100699378693506067827, 8.3325176992594042608724,
     -3.02665838448691571323444e-7},
    {14, 3, 0.0004, -10.4672670714982932744397, 0.000488823712393903035150869,
     2490.59053612251208613134},
    {14, 3, 0.065, -5.71949591388313644017256, 0.0777596519847743338771071,
     11.025105769529760808809},
    {14, 3, 4.42, -7.79006429060418247514041, 2.18418688230008984725966,
     -0.483245366998347706389421},
    {14, 3, 800, -12.7443244787126682405839, 3.6529680365296803652968,
     -0.0000822242550653573074282639},
    {14, 3, 15324, -12.8071593989944707311042, 3.66594897892607816271938,
     -2.27631817501670696455485e-7},
    {14, 180, 0.0004, -10.4670693257255523386798, -2.04937816187568965896125e-6,
     2491.08472067831379995549},
    {14, 180, 0.065, -5.68921919805166473123324, -0.000332904475852855604611915,
     11.4626317745298008318075},
    {14, 180, 4.42, -9.65337657959679927166961, -0.0221029293038836472303558,
     -1.31368299764513841319336},
    {14, 180, 800, -117.570556789937986073345, -0.752834467120181405895692,
     -0.0161936967338941701029113},
    {14, 180, 15324, -131.5984192349764106228, -0.911515307877536979704162,
     -0.0000577016781835615346878727},
    {14, 1123, 0.0004, -10.4677755213861686112027,
     -3.51748189118411715224579e-7, 2489.319233308602484167},
    {14, 1123, 0.065, -5.8039572920094235935389,
     -0.0000571557928751928433161097, 9.69771975676490255408168},
    {14, 1123, 4.42, -17.3710293984633664722496, -0.00387158077355886416717715,
     -3.04061182895062280658097},
    {14, 1123, 800, -640.647914910330698665346, -0.41083032457540509990836,
     -0.282967557637372205015805},
    {14, 1123, 15324, -1011.59993064034851794618, -0.920104682295016979172854,
     -0.00238094023047733949128954},
    {14, 10586, 0.0004, -10.468668475096403358952,
     -3.77357816303515044684135e-8, 2487.08684934922326872119},
    {14, 10586, 0.065, -5.94905895063816048990702,
     -6.13202709485509703057976e-6, 7.46543792757153500263484},
    {14, 10586, 4.42, -27.2227588675497966851323,
     -0.000416806372526964100271534, -5.26603093110764468733629},
    {14, 10586, 800, -2056.93593717552727578896, -0.0701688036945746844270555,
     -1.70965999479388506262815},
    {14, 10586, 15324, -7951.09533692356666073317, -0.590649710084385025903513,
     -0.116267766949827820373742},
    {1525, 0.0000256, 0.0004, -4301.78472746701578211824,
     5.59871348684210526315789e7, -3.58066878763440801575999e6},
    {1525, 0.0000256, 0.065, -11965.4671281234821192651,
     5.95468591985310400826751e7, -23429.1126891147674041279},
    {1525, 0.0000256, 4.42, -18367.3428918005844132604,
     5.95699664793796669412955e7, -339.057082534394502685042},
    {1525, 0.0000256, 800, -24826.2281356385313850842,
     5.9570309593750092999997e7, -0.838976287617641050521018},
    {1525, 0.0000256, 15324, -25707.7170288016100578067,
     5.95703114004829044732209e7, -0.00464306291332368931093164},
    {1525, 0.314, 0.0004, -17.0947387955188070326685, 6.17771997212362846631335,
     -2348.2711549792054388957},
    {1525, 0.314, 0.065, -296.585568286099067098382, 832.7696922844226341529,
     -4001.49218112616940138645},
    {1525, 0.314, 4.42, -4115.10160678526160318827, 4533.61649969457966358017,
     -316.176485619091863713022},
    {1525, 0.314, 800, -10469.9426767525578633112, 4853.78278834474666145767,
     -0.838228516034976412381988},
    {1525, 0.314, 15324, -11350.8644775429644460839, 4855.58840352125852665115,
     -0.00464102415296814454780544},
    {1525, 1.5, 0.0004, -15.5605375416167690037651, 0.270772238514173998044966,
     1484.28030775203773370257},
    {1525, 1.5, 0.065, -74.4509275868752796839738, 42.1842385516506922257721,
     -953.474508334327862701411},
    {1525, 1.5, 4.42, -2072.19103657901875771673, 758.318693693693693693694,
     -251.676626135276647866636},
    {1525, 1.5, 800, -8088.5487996808837996978, 1013.76585568725306716573,
     -0.835410572412918435298525},
    {1525, 1.5, 15324, -8967.33164084000397251235, 1015.56725718573619131513,
     -0.00463332752548632640449661},
    {1525, 3, 0.0004, -15.3575220720668965445137, 0.0676354263875927654090566,
     1991.71749039935673683979},
    {1525, 3, 0.065, -42.4918294753981553150494, 10.759108210984230560087,
     -477.238467087057049216524},
    {1525, 3, 4.42, -1360.54978818586119643475, 302.212039532794249775382,
     -199.675789571126133218244},
    {1525, 3, 800, -7035.84649923785032031894, 505.437941054379410543794,
     -0.831861613850395752926704},
    {1525, 3, 15324, -7911.93122400832367818281, 507.234031447771905787173,
     -0.00462360345575814251935502},
    {1525, 180, 0.0004, -15.1592288635917619923534,
     0.0000166049013718241119958005, 2487.41676922898500772359},
    {1525, 180, 0.065, -10.6183839860155018806964, 0.00269732843386801679640377,
     7.79296281407547767488829},
    {1525, 180, 4.42, -30.7569326420675973698907, 0.179086987432069310390534,
     -5.06066771330890867332748},
    {1525, 180, 800, -1255.10606104316181434609, 6.09977324263038548752834,
     -0.50811617220616562857323},
    {1525, 180, 15324, -1861.51726736161923065929, 7.38547041623667010663915,
     -0.00355560001152251827139362},
    {1525, 1123, 0.0004, -15.1571154863562692876443,
     1.27504753855366555022796e-7, 2492.70020501203972018602},
    {1525, 1123, 0.065, -10.2750368571307051844228,
     0.0000207183306905568286862724, 13.0740397653170432513801},
    {1525, 1123, 4.42, -7.7548168672587285529831, 0.0014034043922188128000047,
     0.0654298516080506295778705},
    {1525, 1123, 800, -30.3901820482837344036952, 0.148921362019218079497891,
     -0.0188047284729684005572389},
    {1525, 1123, 15324, -64.3938471491118556432688, 0.333527576449591366661395,
     -0.000290976140796059790385352},
    {1525, 10586, 0.0004, -15.1575273331900733823398,
     -3.23424061059983902750941e-8, 2491.6705880068073820668},
    {1525, 10586, 0.065, -10.341961135413218918573,
     -5.25560892040125181555838e-6, 12.0444483664247291801249},
    {1525, 10586, 4.42, -12.3018556139457378552047,
     -0.000357234443952593805577031, -0.96243723577972978183147},
    {1525, 10586, 800, -744.016916706287313438782, -0.0601399480019429829354474,
     -0.792452568525027960065769},
    {1525, 10586, 15324, -4300.64559341391477270059,
     -0.506231273465249027592861, -0.080624196979174112177973},
    {10233, 0.0000256, 0.0004, -28781.0708528951564436868,
     3.75682858552631578947368e8, -2.40411931995216805817928e7},
    {10233, 0.0000256, 0.065, -80237.4790550341084402889,
     3.99569192710255653158141e8, -157343.697098361014871643},
    {10233, 0.0000256, 4.42, -123371.163364167739853751,
     3.99724246355043735493297e8, -2307.2799792939510175957},
    {10233, 0.0000256, 800, -173733.655946368116040227,
     3.99726548708750441319986e8, -10.1666354466535988554224},
    {10233, 0.0000256, 15324, -189611.393943800923934812,
     3.9972656083222396519806e8, -0.156271946909903467303064},
    {10233, 0.314, 0.0004, -30.0835346409569126537712,
     41.4607785935398129689956, -30043.5682705897920957967},
    {10233, 0.314, 0.065, -1936.71659175848698855364, 5589.00047056450935246962,
     -26975.8416131385266353558},
    {10233, 0.314, 4.42, -27734.5558869327520485077, 30426.6413450334885998832,
     -2153.7342775414124625757},
    {10233, 0.314, 800, -77398.8345343110590488869, 32575.3861354641233252254,
     -10.1616173371352784115726},
    {10233, 0.314, 15324, -93272.7669155443507051479, 32587.5042326579589686295,
     -0.156258265294963792040417},
    {10233, 1.5, 0.0004, -19.7852231333246169373824, 1.81844841375633164489469,
     -4317.60144720493526427937},
    {10233, 1.5, 0.065, -445.630392408020268115618, 283.300319488817891373802,
     -6515.78789458981693006454},
    {10233, 1.5, 4.42, -14020.6484936315300789489, 5092.70608108108108108108,
     -1720.72113290933110987002},
    {10233, 1.5, 800, -61412.5266404072312643186, 6808.23456019962570180911,
     -10.1426989007295637839166},
    {10233, 1.5, 15324, -77272.0989845637286907938, 6820.33238719780757560928,
     -0.156206593508853883167535},
    {10233, 3, 0.0004, -18.4213731622889640306878, 0.454606052526329822690308,
     -908.658303440053015065049},
    {10233, 3, 0.065, -230.930124889414044823939, 72.3164763458401305057096,
     -3316.44389947819074911539},
    {10233, 3, 4.42, -9239.71902985399623107597, 2031.29380053908355795148,
     -1371.35925605961200776625},
    {10233, 3, 800, -54340.1804040886113152815, 3397.26027397260273972603,
     -10.1188548696496798960218},
    {10233, 3, 15324, -70181.625182690519906018, 3409.33255040125269132903,
     -0.156141261363057784911755},
    {10233, 180, 0.0004, -17.081441774321808557538,
     0.000124110835309254868322515, 2440.94300115825934508112},
    {10233, 180, 0.065, -15.5422428149329860521265, 0.0201607752755949240552023,
     -38.663485376183506373452},
    {10233, 180, 4.42, -235.497379149446597781456, 1.33855872465025485305281,
     -50.3775345819770169658823},
    {10233, 180, 800, -14640.5024630709594885282, 45.5918367346938775510204,
     -7.83648996527639800152069},
    {10233, 180, 15324, -28577.6446971680821285066, 55.2015866873065015479876,
     -0.148587051440887372280679},
    {10233, 1123, 0.0004, -17.0630789988207189145522,
     2.88947340204574456780515e-6, 2486.84988023336341161969},
    {10233, 1123, 0.065, -12.5589181079456256205069,
     0.000469512419380529127691397, 7.22412495831966558459338},
    {10233, 1123, 4.42, -35.4550615786577840007616, 0.0318035174455556831045841,
     -5.75696060521026301882133},
    {10233, 1123, 800, -3343.1303902972739556477, 3.37480997013700672692981,
     -2.98980536947426087305154},
    {10233, 1123, 15324, -11352.0607180353268830661, 7.55829905834770485145599,
     -0.113119180727235553840235},
    {10233, 10586, 0.0004, -17.0607181926565361616021,
     -1.26000103249281886845913e-9, 2492.75189435765523482408},
    {10233, 10586, 0.065, -12.175300607237463446918,
     -2.04748918320454904634379e-7, 13.1257236766090680623184},
    {10233, 10586, 4.42, -9.43079204562662170137108,
     -0.0000139172010501341588531831, 0.116749372001926599593782},
    {10233, 10586, 800, -7.27036619721226178082317, -0.002342942461614156602606,
     0.0000890483066241868231597611},
    {10233, 10586, 15324, -9.3278240351663417156768,
     -0.0197218452194275363359762, -0.0000805948075011592832299515},
};
}  // namespace neg_binomial_2_test_internal

TEST(ProbDistributionsNegativeBinomial2, derivativesPrecomputed) {
  using neg_binomial_2_test_internal::TestValue;
  using neg_binomial_2_test_internal::testValues;
  using stan::math::is_nan;
  using stan::math::neg_binomial_2_log;
  using stan::math::value_of;
  using stan::math::var;
  using stan::test::internal::expect_near_rel_finite;

  for (TestValue t : testValues) {
    int n = t.n;
    var mu(t.mu);
    var phi(t.phi);
    var val = neg_binomial_2_log(n, mu, phi);

    std::vector<var> x;
    x.push_back(mu);
    x.push_back(phi);

    std::vector<double> gradients;
    val.grad(x, gradients);

    for (int i = 0; i < 2; ++i) {
      EXPECT_FALSE(is_nan(gradients[i]));
    }

    auto tolerance = [](double x) { return std::max(fabs(x * 1e-8), 1e-14); };

    EXPECT_NEAR(value_of(val), t.value, tolerance(t.value))
        << "value n = " << n << ", mu = " << t.mu << ", phi = " << t.phi;
    EXPECT_NEAR(gradients[0], t.grad_mu, tolerance(t.grad_mu))
        << "grad_mu n = " << n << ", mu = " << t.mu << ", phi = " << t.phi;
    EXPECT_NEAR(gradients[1], t.grad_phi, tolerance(t.grad_phi))
        << "grad_phi n = " << n << ", mu = " << t.mu << ", phi = " << t.phi;
  }
}

TEST(ProbDistributionsNegBinomial2, derivativesComplexStep) {
  using boost::math::differentiation::complex_step_derivative;
  using stan::math::is_nan;
  using stan::math::neg_binomial_2_log;
  using stan::math::var;
  using stan::test::internal::expect_near_rel_finite;

  std::vector<int> n_to_test = {0, 7, 100, 835, 14238, 385000, 1000000};
  std::vector<double> mu_to_test = {0.8, 8, 24, 271, 2586, 33294};

  auto nb2_log_for_test = [](int n, const std::complex<double>& mu,
                             const std::complex<double>& phi) {
    // Using first-order Taylor expansion of lgamma(a + b*i) around b = 0
    // Which happens to work nice in this case, as b is always 0 or the very
    // small complex step
    auto lgamma_c_approx = [](const std::complex<double>& x) {
      return std::complex<double>(lgamma(x.real()),
                                  x.imag() * boost::math::digamma(x.real()));
    };

    const double n_(n);
    return lgamma_c_approx(n_ + phi) - lgamma(n + 1) - lgamma_c_approx(phi)
           + phi * (log(phi) - log(mu + phi)) - n_ * log(mu + phi)
           + n_ * log(mu);
  };

  for (double mu_dbl : mu_to_test) {
    for (int n : n_to_test) {
      for (double phi_dbl = 1.5; phi_dbl < 1e22; phi_dbl *= 10) {
        var mu(mu_dbl);
        var phi(phi_dbl);
        var val = neg_binomial_2_lpmf(n, mu, phi);

        std::vector<var> x;
        x.push_back(mu);
        x.push_back(phi);

        std::vector<double> gradients;
        val.grad(x, gradients);

        EXPECT_TRUE(value_of(val) < 0)
            << "for n = " << n << ", mu = " << mu_dbl << ", phi = " << phi_dbl;

        for (int i = 0; i < 2; ++i) {
          EXPECT_FALSE(is_nan(gradients[i]));
        }

        auto nb2_log_mu
            = [n, phi_dbl, nb2_log_for_test](const std::complex<double>& mu) {
                return nb2_log_for_test(n, mu, phi_dbl);
              };
        auto nb2_log_phi
            = [n, mu_dbl, nb2_log_for_test](const std::complex<double>& phi) {
                return nb2_log_for_test(n, mu_dbl, phi);
              };
        double complex_step_dmu = complex_step_derivative(nb2_log_mu, mu_dbl);
        double complex_step_dphi
            = complex_step_derivative(nb2_log_phi, phi_dbl);

        std::ostringstream message;
        message << ", n = " << n << ", mu = " << mu_dbl
                << ", phi = " << phi_dbl;

        double tolerance_phi;
        double tolerance_mu;
        // if (phi < phi_cutoff || n < 100000) {
        //   tolerance_phi = std::max(1e-10, fabs(gradients[1]) * 1e-8);
        // } else {
        tolerance_phi = std::max(1e-8, fabs(gradients[1]) * 1e-5);
        // }

        // if (phi < phi_cutoff) {
        tolerance_mu = std::max(1e-10, fabs(gradients[0]) * 1e-8);
        // } else {
        //   tolerance_mu = std::max(1e-8, fabs(gradients[0]) * 1e-5);
        // }

        EXPECT_NEAR(gradients[0], complex_step_dmu, tolerance_mu)
            << "grad_mu" << message.str();

        EXPECT_NEAR(gradients[1], complex_step_dphi, tolerance_phi)
            << "grad_phi" << message.str();
      }
    }
  }
}

TEST(ProbDistributionsNegBinomial2, derivativesZeroOne) {
  using stan::math::var;
  using stan::test::expect_near_rel;

  std::vector<double> mu_to_test = {2.345e-5, 0.2, 13, 150, 1621, 18432, 1e10};
  double phi_start = 1e-8;
  double phi_max = 1e20;
  for (double mu_dbl : mu_to_test) {
    for (double phi_dbl = phi_start; phi_dbl < phi_max;
         phi_dbl *= stan::math::pi()) {
      std::ostringstream msg;
      msg << std::setprecision(20) << ", mu = " << mu_dbl
          << ", phi = " << phi_dbl;

      var mu0(mu_dbl);
      var phi0(phi_dbl);
      var val0 = neg_binomial_2_lpmf(0, mu0, phi0);

      std::vector<var> x0;
      x0.push_back(mu0);
      x0.push_back(phi0);

      std::vector<double> gradients0;
      val0.grad(x0, gradients0);

      var mu1(mu_dbl);
      var phi1(phi_dbl);
      var val1 = neg_binomial_2_lpmf(1, mu1, phi1);

      std::vector<var> x1;
      x1.push_back(mu1);
      x1.push_back(phi1);

      std::vector<double> gradients1;
      val1.grad(x1, gradients1);

      double expected_dmu_0 = -phi_dbl / (mu_dbl + phi_dbl);
      double expected_dphi_0
          = mu_dbl / (mu_dbl + phi_dbl) - log1p(mu_dbl / phi_dbl);
      expect_near_rel("dmu, n = 0 " + msg.str(), gradients0[0], expected_dmu_0);
      expect_near_rel("dphi, n = 0 " + msg.str(), gradients0[1],
                      expected_dphi_0);

      double expected_dmu_1
          = (phi_dbl * (1 - mu_dbl)) / (mu_dbl * (mu_dbl + phi_dbl));
      expect_near_rel("dmu, n = 1 " + msg.str(), gradients1[0], expected_dmu_1);
    }
  }
}
