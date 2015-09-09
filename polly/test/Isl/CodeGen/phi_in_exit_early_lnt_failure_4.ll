; RUN: opt %loadPolly -disable-basicaa -polly-detect-unprofitable -polly-codegen -polly-no-early-exit -S < %s | FileCheck %s
;
; This caused an lnt crash at some point, just verify it will run through and
; produce the PHI node in the exit we are looking for.
;
; CHECK-LABEL:  polly.merge_new_and_old:
; CHECK-NEXT:     %.merge = phi %struct.ImageParameters.11.35.59.83.107.323.539.755.1019.1043.1187.1235.1355.1379.1403.1427.1499.1571.1667.1739.1835.2051.2123.2339.2387.2843.2867.2891.2915.3587.3803.3826* [ %.final_reload, %polly.stmt.for.end.298 ], [ %13, %for.end.298 ]
;
%struct.ImageParameters.11.35.59.83.107.323.539.755.1019.1043.1187.1235.1355.1379.1403.1427.1499.1571.1667.1739.1835.2051.2123.2339.2387.2843.2867.2891.2915.3587.3803.3826 = type { i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, float, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i8**, i8**, i32, i32***, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, [9 x [16 x [16 x i16]]], [5 x [16 x [16 x i16]]], [9 x [8 x [8 x i16]]], [2 x [4 x [16 x [16 x i16]]]], [16 x [16 x i16]], [16 x [16 x i32]], i32****, i32***, i32***, i32***, i32****, i32****, %struct.Picture.8.32.56.80.104.320.536.752.1016.1040.1184.1232.1352.1376.1400.1424.1496.1568.1664.1736.1832.2048.2120.2336.2384.2840.2864.2888.2912.3584.3800.3823*, %struct.Slice.7.31.55.79.103.319.535.751.1015.1039.1183.1231.1351.1375.1399.1423.1495.1567.1663.1735.1831.2047.2119.2335.2383.2839.2863.2887.2911.3583.3799.3822*, %struct.macroblock.9.33.57.81.105.321.537.753.1017.1041.1185.1233.1353.1377.1401.1425.1497.1569.1665.1737.1833.2049.2121.2337.2385.2841.2865.2889.2913.3585.3801.3824*, i32*, i32*, i32, i32, i32, i32, [4 x [4 x i32]], i32, i32, i32, i32, i32, double, i32, i32, i32, i32, i16******, i16******, i16******, i16******, [15 x i16], i32, i32, i32, i32, i32, i32, i32, i32, [6 x [32 x i32]], i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, [1 x i32], i32, i32, [2 x i32], i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, %struct.DecRefPicMarking_s.10.34.58.82.106.322.538.754.1018.1042.1186.1234.1354.1378.1402.1426.1498.1570.1666.1738.1834.2050.2122.2338.2386.2842.2866.2890.2914.3586.3802.3825*, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, double**, double***, i32***, double**, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, [3 x [2 x i32]], [2 x i32], i32, i32, i16, i32, i32, i32, i32, i32 }
%struct.Picture.8.32.56.80.104.320.536.752.1016.1040.1184.1232.1352.1376.1400.1424.1496.1568.1664.1736.1832.2048.2120.2336.2384.2840.2864.2888.2912.3584.3800.3823 = type { i32, i32, [100 x %struct.Slice.7.31.55.79.103.319.535.751.1015.1039.1183.1231.1351.1375.1399.1423.1495.1567.1663.1735.1831.2047.2119.2335.2383.2839.2863.2887.2911.3583.3799.3822*], i32, float, float, float }
%struct.Slice.7.31.55.79.103.319.535.751.1015.1039.1183.1231.1351.1375.1399.1423.1495.1567.1663.1735.1831.2047.2119.2335.2383.2839.2863.2887.2911.3583.3799.3822 = type { i32, i32, i32, i32, i32, i32, %struct.datapartition.3.27.51.75.99.315.531.747.1011.1035.1179.1227.1347.1371.1395.1419.1491.1563.1659.1731.1827.2043.2115.2331.2379.2835.2859.2883.2907.3579.3795.3818*, %struct.MotionInfoContexts.5.29.53.77.101.317.533.749.1013.1037.1181.1229.1349.1373.1397.1421.1493.1565.1661.1733.1829.2045.2117.2333.2381.2837.2861.2885.2909.3581.3797.3820*, %struct.TextureInfoContexts.6.30.54.78.102.318.534.750.1014.1038.1182.1230.1350.1374.1398.1422.1494.1566.1662.1734.1830.2046.2118.2334.2382.2838.2862.2886.2910.3582.3798.3821*, i32, i32*, i32*, i32*, i32, i32*, i32*, i32*, i32 (i32)*, [3 x [2 x i32]] }
%struct.datapartition.3.27.51.75.99.315.531.747.1011.1035.1179.1227.1347.1371.1395.1419.1491.1563.1659.1731.1827.2043.2115.2331.2379.2835.2859.2883.2907.3579.3795.3818 = type { %struct.Bitstream.1.25.49.73.97.313.529.745.1009.1033.1177.1225.1345.1369.1393.1417.1489.1561.1657.1729.1825.2041.2113.2329.2377.2833.2857.2881.2905.3577.3793.3816*, %struct.EncodingEnvironment.2.26.50.74.98.314.530.746.1010.1034.1178.1226.1346.1370.1394.1418.1490.1562.1658.1730.1826.2042.2114.2330.2378.2834.2858.2882.2906.3578.3794.3817, %struct.EncodingEnvironment.2.26.50.74.98.314.530.746.1010.1034.1178.1226.1346.1370.1394.1418.1490.1562.1658.1730.1826.2042.2114.2330.2378.2834.2858.2882.2906.3578.3794.3817 }
%struct.Bitstream.1.25.49.73.97.313.529.745.1009.1033.1177.1225.1345.1369.1393.1417.1489.1561.1657.1729.1825.2041.2113.2329.2377.2833.2857.2881.2905.3577.3793.3816 = type { i32, i32, i8, i32, i32, i8, i8, i32, i32, i8*, i32 }
%struct.EncodingEnvironment.2.26.50.74.98.314.530.746.1010.1034.1178.1226.1346.1370.1394.1418.1490.1562.1658.1730.1826.2042.2114.2330.2378.2834.2858.2882.2906.3578.3794.3817 = type { i32, i32, i32, i32, i32, i8*, i32*, i32, i32 }
%struct.MotionInfoContexts.5.29.53.77.101.317.533.749.1013.1037.1181.1229.1349.1373.1397.1421.1493.1565.1661.1733.1829.2045.2117.2333.2381.2837.2861.2885.2909.3581.3797.3820 = type { [3 x [11 x %struct.BiContextType.4.28.52.76.100.316.532.748.1012.1036.1180.1228.1348.1372.1396.1420.1492.1564.1660.1732.1828.2044.2116.2332.2380.2836.2860.2884.2908.3580.3796.3819]], [2 x [9 x %struct.BiContextType.4.28.52.76.100.316.532.748.1012.1036.1180.1228.1348.1372.1396.1420.1492.1564.1660.1732.1828.2044.2116.2332.2380.2836.2860.2884.2908.3580.3796.3819]], [2 x [10 x %struct.BiContextType.4.28.52.76.100.316.532.748.1012.1036.1180.1228.1348.1372.1396.1420.1492.1564.1660.1732.1828.2044.2116.2332.2380.2836.2860.2884.2908.3580.3796.3819]], [2 x [6 x %struct.BiContextType.4.28.52.76.100.316.532.748.1012.1036.1180.1228.1348.1372.1396.1420.1492.1564.1660.1732.1828.2044.2116.2332.2380.2836.2860.2884.2908.3580.3796.3819]], [4 x %struct.BiContextType.4.28.52.76.100.316.532.748.1012.1036.1180.1228.1348.1372.1396.1420.1492.1564.1660.1732.1828.2044.2116.2332.2380.2836.2860.2884.2908.3580.3796.3819], [4 x %struct.BiContextType.4.28.52.76.100.316.532.748.1012.1036.1180.1228.1348.1372.1396.1420.1492.1564.1660.1732.1828.2044.2116.2332.2380.2836.2860.2884.2908.3580.3796.3819], [3 x %struct.BiContextType.4.28.52.76.100.316.532.748.1012.1036.1180.1228.1348.1372.1396.1420.1492.1564.1660.1732.1828.2044.2116.2332.2380.2836.2860.2884.2908.3580.3796.3819] }
%struct.BiContextType.4.28.52.76.100.316.532.748.1012.1036.1180.1228.1348.1372.1396.1420.1492.1564.1660.1732.1828.2044.2116.2332.2380.2836.2860.2884.2908.3580.3796.3819 = type { i16, i8, i64 }
%struct.TextureInfoContexts.6.30.54.78.102.318.534.750.1014.1038.1182.1230.1350.1374.1398.1422.1494.1566.1662.1734.1830.2046.2118.2334.2382.2838.2862.2886.2910.3582.3798.3821 = type { [2 x %struct.BiContextType.4.28.52.76.100.316.532.748.1012.1036.1180.1228.1348.1372.1396.1420.1492.1564.1660.1732.1828.2044.2116.2332.2380.2836.2860.2884.2908.3580.3796.3819], [4 x %struct.BiContextType.4.28.52.76.100.316.532.748.1012.1036.1180.1228.1348.1372.1396.1420.1492.1564.1660.1732.1828.2044.2116.2332.2380.2836.2860.2884.2908.3580.3796.3819], [3 x [4 x %struct.BiContextType.4.28.52.76.100.316.532.748.1012.1036.1180.1228.1348.1372.1396.1420.1492.1564.1660.1732.1828.2044.2116.2332.2380.2836.2860.2884.2908.3580.3796.3819]], [10 x [4 x %struct.BiContextType.4.28.52.76.100.316.532.748.1012.1036.1180.1228.1348.1372.1396.1420.1492.1564.1660.1732.1828.2044.2116.2332.2380.2836.2860.2884.2908.3580.3796.3819]], [10 x [15 x %struct.BiContextType.4.28.52.76.100.316.532.748.1012.1036.1180.1228.1348.1372.1396.1420.1492.1564.1660.1732.1828.2044.2116.2332.2380.2836.2860.2884.2908.3580.3796.3819]], [10 x [15 x %struct.BiContextType.4.28.52.76.100.316.532.748.1012.1036.1180.1228.1348.1372.1396.1420.1492.1564.1660.1732.1828.2044.2116.2332.2380.2836.2860.2884.2908.3580.3796.3819]], [10 x [5 x %struct.BiContextType.4.28.52.76.100.316.532.748.1012.1036.1180.1228.1348.1372.1396.1420.1492.1564.1660.1732.1828.2044.2116.2332.2380.2836.2860.2884.2908.3580.3796.3819]], [10 x [5 x %struct.BiContextType.4.28.52.76.100.316.532.748.1012.1036.1180.1228.1348.1372.1396.1420.1492.1564.1660.1732.1828.2044.2116.2332.2380.2836.2860.2884.2908.3580.3796.3819]], [10 x [15 x %struct.BiContextType.4.28.52.76.100.316.532.748.1012.1036.1180.1228.1348.1372.1396.1420.1492.1564.1660.1732.1828.2044.2116.2332.2380.2836.2860.2884.2908.3580.3796.3819]], [10 x [15 x %struct.BiContextType.4.28.52.76.100.316.532.748.1012.1036.1180.1228.1348.1372.1396.1420.1492.1564.1660.1732.1828.2044.2116.2332.2380.2836.2860.2884.2908.3580.3796.3819]] }
%struct.macroblock.9.33.57.81.105.321.537.753.1017.1041.1185.1233.1353.1377.1401.1425.1497.1569.1665.1737.1833.2049.2121.2337.2385.2841.2865.2889.2913.3585.3801.3824 = type { i32, i32, i32, [2 x i32], i32, [8 x i32], %struct.macroblock.9.33.57.81.105.321.537.753.1017.1041.1185.1233.1353.1377.1401.1425.1497.1569.1665.1737.1833.2049.2121.2337.2385.2841.2865.2889.2913.3585.3801.3824*, %struct.macroblock.9.33.57.81.105.321.537.753.1017.1041.1185.1233.1353.1377.1401.1425.1497.1569.1665.1737.1833.2049.2121.2337.2385.2841.2865.2889.2913.3585.3801.3824*, i32, [2 x [4 x [4 x [2 x i32]]]], [16 x i8], [16 x i8], i32, i64, [4 x i32], [4 x i32], i64, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i16, double, i32, i32, i32, i32, i32, i32, i32, i32, i32 }
%struct.DecRefPicMarking_s.10.34.58.82.106.322.538.754.1018.1042.1186.1234.1354.1378.1402.1426.1498.1570.1666.1738.1834.2050.2122.2338.2386.2842.2866.2890.2914.3586.3802.3825 = type { i32, i32, i32, i32, i32, %struct.DecRefPicMarking_s.10.34.58.82.106.322.538.754.1018.1042.1186.1234.1354.1378.1402.1426.1498.1570.1666.1738.1834.2050.2122.2338.2386.2842.2866.2890.2914.3586.3802.3825* }

@img = external global %struct.ImageParameters.11.35.59.83.107.323.539.755.1019.1043.1187.1235.1355.1379.1403.1427.1499.1571.1667.1739.1835.2051.2123.2339.2387.2843.2867.2891.2915.3587.3803.3826*, align 8

; Function Attrs: nounwind uwtable
define void @intrapred_luma() #0 {
entry:
  %PredPel = alloca [13 x i16], align 16
  br label %for.body

for.body:                                         ; preds = %for.body, %entry
  br i1 undef, label %for.body, label %for.body.262

for.body.262:                                     ; preds = %for.body
  %0 = load %struct.ImageParameters.11.35.59.83.107.323.539.755.1019.1043.1187.1235.1355.1379.1403.1427.1499.1571.1667.1739.1835.2051.2123.2339.2387.2843.2867.2891.2915.3587.3803.3826*, %struct.ImageParameters.11.35.59.83.107.323.539.755.1019.1043.1187.1235.1355.1379.1403.1427.1499.1571.1667.1739.1835.2051.2123.2339.2387.2843.2867.2891.2915.3587.3803.3826** @img, align 8
  br label %for.body.280

for.body.280:                                     ; preds = %for.body.280, %for.body.262
  %indvars.iv66 = phi i64 [ 0, %for.body.262 ], [ %indvars.iv.next67, %for.body.280 ]
  %arrayidx282 = getelementptr inbounds [13 x i16], [13 x i16]* %PredPel, i64 0, i64 1
  %arrayidx283 = getelementptr inbounds i16, i16* %arrayidx282, i64 %indvars.iv66
  %1 = load i16, i16* %arrayidx283, align 2
  %arrayidx289 = getelementptr inbounds %struct.ImageParameters.11.35.59.83.107.323.539.755.1019.1043.1187.1235.1355.1379.1403.1427.1499.1571.1667.1739.1835.2051.2123.2339.2387.2843.2867.2891.2915.3587.3803.3826, %struct.ImageParameters.11.35.59.83.107.323.539.755.1019.1043.1187.1235.1355.1379.1403.1427.1499.1571.1667.1739.1835.2051.2123.2339.2387.2843.2867.2891.2915.3587.3803.3826* %0, i64 0, i32 47, i64 0, i64 2, i64 %indvars.iv66
  store i16 %1, i16* %arrayidx289, align 2
  %indvars.iv.next67 = add nuw nsw i64 %indvars.iv66, 1
  br i1 false, label %for.body.280, label %for.end.298

for.end.298:                                      ; preds = %for.body.280
  %2 = load %struct.ImageParameters.11.35.59.83.107.323.539.755.1019.1043.1187.1235.1355.1379.1403.1427.1499.1571.1667.1739.1835.2051.2123.2339.2387.2843.2867.2891.2915.3587.3803.3826*, %struct.ImageParameters.11.35.59.83.107.323.539.755.1019.1043.1187.1235.1355.1379.1403.1427.1499.1571.1667.1739.1835.2051.2123.2339.2387.2843.2867.2891.2915.3587.3803.3826** @img, align 8
  br label %for.body.310

for.body.310:                                     ; preds = %for.body.310, %for.end.298
  %indvars.iv = phi i64 [ 0, %for.end.298 ], [ %indvars.iv.next, %for.body.310 ]
  %arrayidx312 = getelementptr inbounds [13 x i16], [13 x i16]* %PredPel, i64 0, i64 9
  %arrayidx313 = getelementptr inbounds i16, i16* %arrayidx312, i64 %indvars.iv
  %3 = load i16, i16* %arrayidx313, align 2
  %arrayidx322 = getelementptr inbounds %struct.ImageParameters.11.35.59.83.107.323.539.755.1019.1043.1187.1235.1355.1379.1403.1427.1499.1571.1667.1739.1835.2051.2123.2339.2387.2843.2867.2891.2915.3587.3803.3826, %struct.ImageParameters.11.35.59.83.107.323.539.755.1019.1043.1187.1235.1355.1379.1403.1427.1499.1571.1667.1739.1835.2051.2123.2339.2387.2843.2867.2891.2915.3587.3803.3826* %2, i64 0, i32 47, i64 1, i64 %indvars.iv, i64 1
  store i16 %3, i16* %arrayidx322, align 2
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  br i1 false, label %for.body.310, label %for.end.328

for.end.328:                                      ; preds = %for.body.310
  ret void
}
