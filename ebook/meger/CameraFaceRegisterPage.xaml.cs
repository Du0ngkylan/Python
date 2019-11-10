using Fukumaro.Common;
using Fukumaro.Models;
using Fukumaro.Utils;
using FukumaroShared.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.Graphics.Display;
using Windows.Graphics.Imaging;
using Windows.Media.Capture;
using Windows.Media.MediaProperties;
using Windows.Storage;
using Windows.Storage.FileProperties;
using Windows.Storage.Streams;
using Windows.System;
using Windows.System.Display;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;
using FaceWrapperCLI;
using System.IO;
using Windows.Media.FaceAnalysis;
using Windows.Media;
using System.Runtime.InteropServices.WindowsRuntime;

namespace Fukumaro.Pages
{
    public class CropBitmap
    {
        /// <summary>
        /// Get a cropped bitmap from a image file.
        /// </summary>
        /// <param name="originalImageFile">
        /// The original image file.
        /// </param>
        /// <param name="startPoint">
        /// The start point of the region to be cropped.
        /// </param>
        /// <param name="corpSize">
        /// The size of the region to be cropped.
        /// </param>
        /// <returns>
        /// The cropped image.
        /// </returns>
        /// 
        async public static Task<SoftwareBitmap> CorpSoftwareBitmap(SoftwareBitmap softwareBitmap, DetectedFace face)
        {
            byte[] bytes = null;
            SoftwareBitmap cropped = new SoftwareBitmap(softwareBitmap.BitmapPixelFormat, (int)face.FaceBox.Width, (int)face.FaceBox.Height);
            try
            {
                IRandomAccessStream stream = new InMemoryRandomAccessStream();
                {
                    // Create an encoder with the desired format
                    BitmapEncoder encoder = await BitmapEncoder.CreateAsync(BitmapEncoder.BmpEncoderId, stream);
                    encoder.SetSoftwareBitmap(softwareBitmap);
                    encoder.IsThumbnailGenerated = false;
                    await encoder.FlushAsync();

                    BitmapDecoder decoder = await BitmapDecoder.CreateAsync(stream);
                    bytes = await CropBitmap.GetPixelData(decoder, face.FaceBox.X, face.FaceBox.Y, face.FaceBox.Width, face.FaceBox.Height);

                }
            }

            catch (Exception ex)
            {
                //Debug.WriteLine(ex);
            }

            cropped.Dispose();
            cropped = SoftwareBitmap.CreateCopyFromBuffer(bytes.AsBuffer(), BitmapPixelFormat.Bgra8, (int)face.FaceBox.Width, (int)face.FaceBox.Height);
            return cropped;
        }

        async public static Task<WriteableBitmap> GetCroppedBitmapAsync(StorageFile originalImageFile,
            Point startPoint, Size corpSize, double scale)
        {
            if (double.IsNaN(scale) || double.IsInfinity(scale))
            {
                scale = 1;
            }

            // Convert start point and size to integer.
            uint startPointX = (uint)Math.Floor(startPoint.X * scale);
            uint startPointY = (uint)Math.Floor(startPoint.Y * scale);
            uint height = (uint)Math.Floor(corpSize.Height * scale);
            uint width = (uint)Math.Floor(corpSize.Width * scale);

            using (IRandomAccessStream stream = await originalImageFile.OpenReadAsync())
            {

                // Create a decoder from the stream. With the decoder, we can get 
                // the properties of the image.
                BitmapDecoder decoder = await BitmapDecoder.CreateAsync(stream);

                // The scaledSize of original image.
                uint scaledWidth = (uint)Math.Floor(decoder.PixelWidth * scale);
                uint scaledHeight = (uint)Math.Floor(decoder.PixelHeight * scale);


                // Refine the start point and the size. 
                if (startPointX + width > scaledWidth)
                {
                    startPointX = scaledWidth - width;
                }

                if (startPointY + height > scaledHeight)
                {
                    startPointY = scaledHeight - height;
                }

                // Get the cropped pixels.
                byte[] pixels = await GetPixelData(decoder, startPointX, startPointY, width, height,
                    scaledWidth, scaledHeight);

                // Stream the bytes into a WriteableBitmap
                WriteableBitmap cropBmp = new WriteableBitmap((int)width, (int)height);
                Stream pixStream = cropBmp.PixelBuffer.AsStream();
                pixStream.Write(pixels, 0, (int)(width * height * 4));

                return cropBmp;
            }
        }
        async public static Task<byte[]> CropSoftwareBitmapToBufferAsync(SoftwareBitmap softwareBitmap)
        {
            byte[] bytes = null;

            try
            {
                IRandomAccessStream stream = new InMemoryRandomAccessStream();
                {

                    // Create an encoder with the desired format
                    BitmapEncoder encoder = await BitmapEncoder.CreateAsync(BitmapEncoder.BmpEncoderId, stream);
                    encoder.SetSoftwareBitmap(softwareBitmap);
                    encoder.IsThumbnailGenerated = false;
                    await encoder.FlushAsync();

                    bytes = new byte[stream.Size];

                    // This returns IAsyncOperationWithProgess, so you can add additional progress handling
                    await stream.ReadAsync(bytes.AsBuffer(), (uint)stream.Size, Windows.Storage.Streams.InputStreamOptions.None);
                }
            }

            catch (Exception ex)
            {
                //Debug.WriteLine(ex);
            }

            return bytes;
        }

        public static async Task CropFaceAsync(SoftwareBitmap softwareBitmap, IList<DetectedFace> faces)
        {
            using (SoftwareBitmap convertedSource = SoftwareBitmap.Convert(softwareBitmap, BitmapPixelFormat.Bgra8))
            {
                foreach (DetectedFace face in faces)
                {
                    byte[] bytes = await GetImageCop(convertedSource, face);
                }
            }

        }

        async public static Task<byte[]> GetImageCop(SoftwareBitmap softwareBitmap, DetectedFace face)
        {
            byte[] bytes = null;
            var picturesLibrary = await StorageLibrary.GetLibraryAsync(KnownLibraryId.Pictures);
            // Fall back to the local app storage if the Pictures Library is not available
            StorageFolder _captureFolder = picturesLibrary.SaveFolder ?? ApplicationData.Current.LocalFolder;
            StorageFile newImgFile = await _captureFolder.CreateFileAsync("tes.jpg", CreationCollisionOption.GenerateUniqueName);
            try
            {
                IRandomAccessStream stream = new InMemoryRandomAccessStream();
                {

                    // Create an encoder with the desired format
                    BitmapEncoder encoder = await BitmapEncoder.CreateAsync(BitmapEncoder.BmpEncoderId, stream);
                    encoder.SetSoftwareBitmap(softwareBitmap);
                    encoder.IsThumbnailGenerated = false;
                    //byte[] bytes = CropBitmap.GetPixelData(encoder, face.FaceBox.X, face.FaceBox.Y, face.FaceBox.Width, face.FaceBox.Height);
                    await encoder.FlushAsync();

                    BitmapDecoder decoder = await BitmapDecoder.CreateAsync(stream);
                    bytes = await CropBitmap.GetPixelData(decoder, face.FaceBox.X, face.FaceBox.Y, face.FaceBox.Width, face.FaceBox.Height);

                    using (IRandomAccessStream newImgFileStream = await newImgFile.OpenAsync(FileAccessMode.ReadWrite))
                    {
                        Guid encoderID = Guid.Empty;


                        switch (newImgFile.FileType.ToLower())
                        {
                            case ".png":
                                encoderID = BitmapEncoder.PngEncoderId;
                                break;
                            case ".bmp":
                                encoderID = BitmapEncoder.BmpEncoderId;
                                break;
                            default:
                                encoderID = BitmapEncoder.JpegEncoderId;
                                break;
                        }


                        // Create a bitmap encoder 


                        BitmapEncoder bmpEncoder = await BitmapEncoder.CreateAsync(
                            encoderID,
                            newImgFileStream);


                        // Set the pixel data to the cropped image. 
                        bmpEncoder.SetPixelData(
                            BitmapPixelFormat.Bgra8,
                            BitmapAlphaMode.Straight,
                            face.FaceBox.Width,
                            face.FaceBox.Height,
                            decoder.DpiX,
                            decoder.DpiY,
                            bytes);


                        // Flush the data to file. 
                        await bmpEncoder.FlushAsync();
                    }
                    //bytes = new byte[stream.Size];

                    //// This returns IAsyncOperationWithProgess, so you can add additional progress handling
                    //await stream.ReadAsync(bytes.AsBuffer(), (uint)stream.Size, Windows.Storage.Streams.InputStreamOptions.None);
                }
            }

            catch (Exception ex)
            {
                //Debug.WriteLine(ex);
            }

            return bytes;
        }

        async public static Task<byte[]> SaveSoftwareBitmapToBufferAsync(SoftwareBitmap softwareBitmap)
        {
            byte[] bytes = null;

            try
            {
                IRandomAccessStream stream = new InMemoryRandomAccessStream();
                {

                    // Create an encoder with the desired format
                    BitmapEncoder encoder = await BitmapEncoder.CreateAsync(BitmapEncoder.BmpEncoderId, stream);
                    encoder.SetSoftwareBitmap(softwareBitmap);
                    encoder.IsThumbnailGenerated = false;
                    await encoder.FlushAsync();

                    bytes = new byte[stream.Size];

                    // This returns IAsyncOperationWithProgess, so you can add additional progress handling
                    await stream.ReadAsync(bytes.AsBuffer(), (uint)stream.Size, Windows.Storage.Streams.InputStreamOptions.None);
                }
            }

            catch (Exception ex)
            {
                //Debug.WriteLine(ex);
            }

            return bytes;
        }

        /// <summary>
        /// Use BitmapTransform to define the region to crop, and then get the pixel data in the region
        /// </summary>
        async static private Task<byte[]> GetPixelData(BitmapDecoder decoder, uint startPointX, uint startPointY,
            uint width, uint height)
        {
            return await GetPixelData(decoder, startPointX, startPointY, width, height,
                decoder.PixelWidth, decoder.PixelHeight);
        }

        /// <summary>
        /// Use BitmapTransform to define the region to crop, and then get the pixel data in the region.
        /// If you want to get the pixel data of a scaled image, set the scaledWidth and scaledHeight
        /// of the scaled image.
        /// </summary>
        async static private Task<byte[]> GetPixelData(BitmapDecoder decoder, uint startPointX, uint startPointY,
            uint width, uint height, uint scaledWidth, uint scaledHeight)
        {
            BitmapTransform transform = new BitmapTransform();
            BitmapBounds bounds = new BitmapBounds();
            bounds.X = startPointX;
            bounds.Y = startPointY;
            bounds.Height = height;
            bounds.Width = width;
            transform.Bounds = bounds;

            transform.ScaledWidth = scaledWidth;
            transform.ScaledHeight = scaledHeight;

            // Get the cropped pixels within the bounds of transform.
            PixelDataProvider pix = await decoder.GetPixelDataAsync(
                BitmapPixelFormat.Bgra8,
                BitmapAlphaMode.Straight,
                transform,
                ExifOrientationMode.IgnoreExifOrientation,
                ColorManagementMode.ColorManageToSRgb);
            byte[] pixels = pix.DetachPixelData();
            return pixels;
        }
    }

    /// <summary>
    /// カメラ設定ページ
    /// </summary>
    public sealed partial class CameraFaceRegisterPage : BasePage
    {
        #region 内部クラス：StreamResolutionクラス
        /// <summary>
        /// 内部クラス：StreamResolutionクラス
        /// </summary>
        private class StreamResolution
        {
            private IMediaEncodingProperties _properties;

            public StreamResolution(IMediaEncodingProperties properties)
            {
                if (properties == null)
                {
                    throw new ArgumentNullException(nameof(properties));
                }

                // Only handle ImageEncodingProperties and VideoEncodingProperties, which are the two types that GetAvailableMediaStreamProperties can return
                if (!(properties is ImageEncodingProperties) && !(properties is VideoEncodingProperties))
                {
                    throw new ArgumentException("Argument is of the wrong type. Required: " + typeof(ImageEncodingProperties).Name
                        + " or " + typeof(VideoEncodingProperties).Name + ".", nameof(properties));
                }
                // Store the actual instance of the IMediaEncodingProperties for setting them later
                _properties = properties;
            }

            public uint Width
            {
                get
                {
                    if (_properties is ImageEncodingProperties)
                    {
                        return (_properties as ImageEncodingProperties).Width;
                    }
                    else if (_properties is VideoEncodingProperties)
                    {
                        return (_properties as VideoEncodingProperties).Width;
                    }

                    return 0;
                }
            }

            public uint Height
            {
                get
                {
                    if (_properties is ImageEncodingProperties)
                    {
                        return (_properties as ImageEncodingProperties).Height;
                    }
                    else if (_properties is VideoEncodingProperties)
                    {
                        return (_properties as VideoEncodingProperties).Height;
                    }

                    return 0;
                }
            }

            public uint FrameRate
            {
                get
                {
                    if (_properties is VideoEncodingProperties)
                    {
                        if ((_properties as VideoEncodingProperties).FrameRate.Denominator != 0)
                        {
                            return (_properties as VideoEncodingProperties).FrameRate.Numerator / (_properties as VideoEncodingProperties).FrameRate.Denominator;
                        }
                    }

                    return 0;
                }
            }

            public double AspectRatio
            {
                get { return Math.Round((Height != 0) ? (Width / (double)Height) : double.NaN, 2); }
            }

            public IMediaEncodingProperties EncodingProperties
            {
                get { return _properties; }
            }
        }
        #endregion

        /// <summary>
        /// ログ出力インスタンス
        /// </summary>
        private LoggerUtils logger = new LoggerUtils("CameraPage");

        /// <summary>
        /// 画面
        /// </summary>
        private DisplayRequest _displayRequest = null;

        /// <summary>
        /// 動画キャプチャー
        /// </summary>
        private MediaCapture _mediaCapture = null;

        /// <summary>
        /// デバイス一覧
        /// </summary>
        private List<DeviceInformation> devices = null;

        /// <summary>
        /// ハンドラ登録フラグ
        /// </summary>
        private bool isHandleRegistered = false;


        FaceDetector faceTracker1;

        #region プロパティ
        /// <summary>
        /// プレビューフラグ属性
        /// </summary>
        private bool isPreviewing
        {
            get
            {
                if (_mediaCapture == null)
                {
                    return false;
                }
                else
                {
                    try
                    {
                        return _mediaCapture.CameraStreamState == Windows.Media.Devices.CameraStreamState.Streaming;
                    }
                    catch { }
                    return false;
                }
            }
        }

        /// <summary>
        /// デバイスエラー認識種別属性
        /// </summary>
        private DeviceErrorType DeviceError { get; set; } = DeviceErrorType.Initial;

        /// <summary>
        /// キャプチャデバイスID属性
        /// </summary>
        private string CaptureStartDeviceId { get; set; } = String.Empty;

        /// <summary>
        /// メインページ属性
        /// </summary>
        private MainPage mainPage
        {
            get
            {
                return (Window.Current.Content as MainPage);
            }
        }
        #endregion

        /// <summary>
        /// コンストラクタ
        /// </summary>
        public CameraFaceRegisterPage()
        {
            this.InitializeComponent();
        }

        private static string MyCloudPublicFolder = Def.MY_CLOUD_OPEN_DATA_FOLDER_NAME;

        /// <summary>
        /// メインページのオーバーレイ表示/非表示入り替えイベント
        /// </summary>
        /// <param name="sender"></param>
        private void MainPage_OverlayClose(object sender)
        {
            mainPage?.NavigateBlankPagePreviewFrame();
        }

        /// <summary>
        /// 画面遷移前処理
        /// </summary>
        /// <param name="e"></param>
        public async override void OnInitialize(NavigationEventArgs e)
        {
            // 音声認識を停止する
            StopSpeechRecognition();

            // メインページのオーバーレイ表示/非表示入り替えイベントを関連付け設定
            mainPage.EventOverlayClose += MainPage_OverlayClose;
            if (fukumaroData.IsManekoTime)
            {
                TopPage topPage = (TopPage)MainFrame.GetMainPage();
                topPage.StopManekoTime();
            }

            InitializePage();
            if (_displayRequest == null)
            {
                _displayRequest = new DisplayRequest();
            }

            AddIntent(MessageIntent.IntentIndex.UPDATE_CAMERA_DEVICE, OnReceiveUpdateCameraDevice);

            Application.Current.Suspending += Application_Suspending;
            Application.Current.Resuming += Application_Resuming;
            Window.Current.VisibilityChanged += Window_VisibilityChanged;
            _isActivePage = true;
            faceTracker1 = await FaceDetector.CreateAsync();
            await SetUpBasedOnStateAsync();
        }

        /// <summary>
        /// 画面遷移前処理
        /// </summary>
        /// <param name="e"></param>
        public async override void OnFinalize(NavigationEventArgs e)
        {
            // メインページのオーバーレイ表示/非表示入り替えイベントを関連付け解除
            mainPage.EventOverlayClose -= MainPage_OverlayClose;

            Application.Current.Suspending -= Application_Suspending;
            Application.Current.Resuming -= Application_Resuming;
            Window.Current.VisibilityChanged -= Window_VisibilityChanged;
            RemoveIntent(MessageIntent.IntentIndex.UPDATE_CAMERA_DEVICE, OnReceiveUpdateCameraDevice);
            _isActivePage = false;
            await SetUpBasedOnStateAsync();

            // カメラ機器終了処理
            CameraFinalize();
        }

        /// <summary>
        /// カメラデバイス更新時
        /// </summary>
        /// <param name="intent"></param>
        private async void OnReceiveUpdateCameraDevice(MessageIntent intent)
        {
            bool direction = (bool)intent.GetValue("direction");
            string deviceId = (string)intent.GetValue("deviceId");

            // プレビュー中の対象のカメラデバイスが取り外された場合
            if ((!direction) && (string.Compare(this.CaptureStartDeviceId, deviceId, true) == 0))
            {
                // デバイスエラーの処理を発行させる
                this.DeviceError = DeviceErrorType.DeviceError;
            }

            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, async () =>
            {
                // Todo: 
            });
        }

        /// <summary>
        /// 戻るボタンクリック
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public override void OnBackRequested(object sender, BackRequestedEventArgs e)
        {
            // PairingPageには戻らない.
            MainPageNavigate();
        }

        /// <summary>
        /// 初期化処理
        /// </summary>
        private void InitializePage()
        {
            SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility = AppViewBackButtonVisibility.Collapsed;
            CameraInitialize();
        }

        /// <summary>
        /// カメラ機器初期化処理
        /// </summary>
        /// <param name="deviceId">カメラデバイスID</param>
        private async void CameraInitialize()
        {
            logger.Debug("CameraInitialize : start");
            if (!fukumaroData.HouseWatching)
            {
                //  カメラデバイス検索
                bool ret = await SearchCameraDevice(fukumaroData.CameraDeviceId);
                if (ret)
                {
                    await SetupCameraAsync(fukumaroData.CameraDeviceId);
                }
                else
                {
                    // カメラが認識できなかった
                    MainPageErrorNavigate(ResponseId.MY_PICTURES_PHOTOGRAPHY_FAILURE_NOCAMERA);
                }
            }
            else
            {
                // 留守番モード中のため、写真撮影出来ません
                MainPageErrorNavigate(ResponseId.MY_PICTURES_PHOTOGRAPHY_FAILURE_DETECT);
            }
            logger.Debug("CameraInitialize : end");
        }

        /// <summary>
        /// カメラ機器終了処理
        /// </summary>
        private async void CameraFinalize()
        {
            logger.Debug("CameraFinalize : start");
            await CleanupCameraAsync();
            devices = null;
            _displayRequest = null;
            logger.Debug("CameraFinalize : end");
        }

        /// <summary>
        /// カメラ一覧をソートする.
        /// </summary>
        private List<DeviceInformation> SortCameraList(DeviceInformationCollection devices)
        {
            List<DeviceInformation> deviceArray = new List<DeviceInformation>(devices);
            // IR顔認証用カメラ"FJ IR Sensor"は除外する
            foreach (var device in devices)
            {
                if (device.Name == "FJ IR Sensor")
                {
                    deviceArray.Remove(device);
                }
            }
            // 並び替え.
            deviceArray.Sort(Comparison);
            return deviceArray;

        }

        /// <summary>
        /// DeviceInformationを比較する.
        /// </summary>
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <returns></returns>
        private static int Comparison(DeviceInformation a, DeviceInformation b)
        {
            // "FJ Camera"を先頭
            string fjCamera = "FJ Camera";
            if (a.Name == fjCamera)
            {
                return -1;
            }
            else if (b.Name == fjCamera)
            {
                return 1;
            }

            // "FJ ○M Camera"が次.
            string fjMCamera = @"FJ .*M Camera";
            if (Regex.IsMatch(a.Name, fjMCamera))
            {
                // 両方とも"FJ ○M Camera"の場合、○の値でソートする.
                if (Regex.IsMatch(b.Name, fjMCamera))
                {
                    return a.Name.CompareTo(b.Name);
                }
                return -1;
            }
            else if (Regex.IsMatch(b.Name, fjMCamera))
            {
                return 1;
            }
            return -1;
        }

        /// <summary>
        /// カメラデバイス更新処理
        /// </summary>
        /// <param name="selectedCamera"></param>
        /// <returns></returns>
        private async Task<bool> SearchCameraDevice(string selectedCamera)
        {
            DeviceInformationCollection deviceCollection = await DeviceInformation.FindAllAsync(DeviceClass.VideoCapture);
            if (deviceCollection.Count > 0)
            {
                //  ソート
                devices = SortCameraList(deviceCollection);
                int count = 0;
                int position = -1;
                if (string.IsNullOrEmpty(selectedCamera))
                {
                    fukumaroData.CameraDeviceId = SortCameraList(deviceCollection)[0].Id;
                    return true;
                }
                foreach (var device in devices)
                {
                    string deviceId = device.Id;
                    if (selectedCamera != null)
                    {
                        if (deviceId.IndexOf(selectedCamera) != -1)
                        {
                            position = count;
                            break;
                        }
                    }
                    count++;
                }
                if (position != -1)
                {
                    //ComboCamera.SelectedIndex = position;
                    return true;
                }
                else
                {
                    //ComboCamera.SelectedIndex = 0;
                    return false;
                }
            }
            else
            {
                devices = null;
                return false;
            }
            //return deviceCollection.Count > 0 ? true : false;
        }

        /// <summary>
        /// カメラリソース設定処理
        /// </summary>
        /// <param name="deviceId"></param>
        /// <returns></returns>
        private async Task SetupCameraAsync(string deviceId)
        {
            logger.Debug("Setup Camera : deviceId : " + deviceId);

            // デバイスエラー認識種別を初期化
            this.DeviceError = DeviceErrorType.NoError;

            _isUIActive = true;
            MediaCaptureInitializationSettings setting = new MediaCaptureInitializationSettings
            {
                VideoDeviceId = deviceId,
                StreamingCaptureMode = StreamingCaptureMode.Video
            };
            try
            {
                _mediaCapture = new MediaCapture();
                await _mediaCapture.InitializeAsync(setting);
                this.CaptureStartDeviceId = deviceId;
                _mediaCapture.Failed += MediaCapture_OnFailed;
                _mediaCapture.CameraStreamStateChanged += MediaCapture_CameraStreamStateChanged;
                _mediaCapture.ThermalStatusChanged += MediaCapture_ThermalStatusChanged;
                isHandleRegistered = true;

                _displayRequest.RequestActive();
                DisplayInformation.AutoRotationPreferences = DisplayOrientations.Landscape;

                CameraCapture.Source = _mediaCapture;
                await _mediaCapture.StartPreviewAsync();
                logger.Debug("SetupCameraAsync : success : " + CameraCapture.Source.CameraStreamState);

                await SetStreamResolution();

                // 画像撮影呼び出し処理
                await TakePicture();
            }
            catch (UnauthorizedAccessException e)
            {
                logger.Error("SetupCameraAsync : " + e.Message);

                this.DeviceError = DeviceErrorType.DeviceAccessError;
            }
            catch (NotPossessException e)
            {
                logger.Info("SetupCameraAsync : " + e.Message);

                this.DeviceError = DeviceErrorType.DeviceNotPossess;
            }
            catch (Exception e)
            {
                logger.Error("SetupCameraAsync : " + e.Message);

                this.DeviceError = DeviceErrorType.DeviceError;
            }

            // デバイスエラー処理を実行
            if (this.DeviceErrorExecute())
            {
                // エラー処理が実行されている場合は、後続処理を行わない
                return;
            }
        }

        private void MediaCapture_ThermalStatusChanged(MediaCapture sender, object args)
        {
            logger.Debug("ThermalStatusChanged : " + sender.ThermalStatus);
        }

        /// <summary>
        /// カメラ状態遷移通知
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private void MediaCapture_CameraStreamStateChanged(MediaCapture sender, object args)
        {
            logger.Debug("CameraStreamStateChanged : " + sender.CameraStreamState);
        }

        /// <summary>
        /// カメラ異常検出処理
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="errorEventArgs"></param>
        private void MediaCapture_OnFailed(MediaCapture sender, MediaCaptureFailedEventArgs errorEventArgs)
        {
            uint decCode = errorEventArgs.Code;
            string hexCode = "0x" + decCode.ToString("X");
            logger.Error($"Detect Media Capture Failed : Camera {sender.CameraStreamState} : [{hexCode}]{errorEventArgs.Message}");

            // エラーを判別するためにコード変換
            MediaCaptureFailedErrCode deviceErrCode = (MediaCaptureFailedErrCode)Enum.ToObject(typeof(MediaCaptureFailedErrCode), decCode);

            // エラーコードによる処理分岐
            switch (deviceErrCode)
            {
                // アクセスが拒否されました
                case MediaCaptureFailedErrCode.DEVICE_ACCESS:
                    this.DeviceError = DeviceErrorType.DeviceAccessError;
                    break;

                // 他アプリが使用中でデバイスが占有できず
                case MediaCaptureFailedErrCode.DEVICE_NOT_POSSESS:
                    this.DeviceError = DeviceErrorType.DeviceNotPossess;
                    break;

                case MediaCaptureFailedErrCode.DEVICE_PREEMTED:
                    this.DeviceError = DeviceErrorType.DevicePreempted;
                    break;
                default:
                    this.DeviceError = DeviceErrorType.DeviceError;
                    break;
            }
        }

        /// <summary>
        /// カメラリソース解放処理
        /// </summary>
        /// <returns></returns>
        private async Task CleanupCameraAsync()
        {
            if (_mediaCapture != null)
            {
                logger.Debug("CleanupCameraAsync Start");

                this.CameraCapture.Source = null;

                if (isPreviewing)
                {
                    await _mediaCapture.StopPreviewAsync();
                    logger.Debug("CleanupCameraAsync Stop Preview");
                }

                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    if (_displayRequest != null)
                    {
                        try
                        {
                            _displayRequest.RequestRelease();
                            logger.Debug("CleanupCameraAsync Release DisplayRequest");
                        }
                        catch (InvalidOperationException e)
                        {
                            logger.Warn("CleanupCameraAsync request Warn : " + e.Message);
                        }
                        catch (Exception e)
                        {
                            logger.Error("CleanupCameraAsync Release DisplayRequest", e);
                            Console.WriteLine(e.Message);
                            _displayRequest = null;
                        }
                    }

                    if (isHandleRegistered)
                    {
                        _mediaCapture.Failed -= MediaCapture_OnFailed;
                        _mediaCapture.CameraStreamStateChanged -= MediaCapture_CameraStreamStateChanged;
                        _mediaCapture.ThermalStatusChanged -= MediaCapture_ThermalStatusChanged;
                        isHandleRegistered = false;
                    }

                    _mediaCapture.Dispose();
                    logger.Debug("CleanupCameraAsync Dispose");
                    _mediaCapture = null;
                });
            }
        }

        private async void Window_VisibilityChanged(object sender, VisibilityChangedEventArgs e)
        {
            logger.Debug("VisibilityChanged " + e.Visible);
            await SetUpBasedOnStateAsync();
        }

        private void Application_Resuming(object sender, object e)
        {
            if (Frame.CurrentSourcePageType == typeof(CameraSettingPage))
            {
                logger.Debug("Resume");
                _isSuspending = false;
                var task = Dispatcher.RunAsync(CoreDispatcherPriority.High, async () =>
                {
                    await SetUpBasedOnStateAsync();
                });
            }
        }

        private void Application_Suspending(object sender, SuspendingEventArgs e)
        {
            if (Frame.CurrentSourcePageType == typeof(CameraSettingPage))
            {
                logger.Debug("Suspend");
                _isSuspending = true; // TODO:サンプルだとfalse

                var deferral = e.SuspendingOperation.GetDeferral();
                var task = Dispatcher.RunAsync(CoreDispatcherPriority.High, async () =>
                {
                    await SetUpBasedOnStateAsync();
                    deferral.Complete();
                });
            }
        }

        #region from CameraStarterKit
        // UI state
        private Task _setupTask = Task.CompletedTask;
        private bool _isSuspending;
        private bool _isActivePage;
        private bool _isUIActive;

        /// <summary>
        /// SetStreamResolution
        /// </summary>
        /// <returns></returns>
        private async Task SetStreamResolution()
        {
            // Query all preview properties of the device 
            IEnumerable<StreamResolution> allStreamProperties = _mediaCapture.VideoDeviceController.GetAvailableMediaStreamProperties(MediaStreamType.VideoPreview).Select(x => new StreamResolution(x));
            // Order them by resolution then frame rate
            allStreamProperties = allStreamProperties.OrderByDescending(x => x.Height * x.Width).ThenByDescending(x => x.FrameRate);
            if (allStreamProperties.Count() > 0)
            {
                var encodingProperties = allStreamProperties.First().EncodingProperties;
                await _mediaCapture.VideoDeviceController.SetMediaStreamPropertiesAsync(MediaStreamType.VideoPreview, encodingProperties);
            }

            // Query all preview properties of the device 
            allStreamProperties = _mediaCapture.VideoDeviceController.GetAvailableMediaStreamProperties(MediaStreamType.Photo).Select(x => new StreamResolution(x));
            // Order them by resolution then frame rate
            allStreamProperties = allStreamProperties.OrderByDescending(x => x.Height * x.Width).ThenByDescending(x => x.FrameRate);
            if (allStreamProperties.Count() > 0)
            {
                var encodingProperties = allStreamProperties.First().EncodingProperties;
                await _mediaCapture.VideoDeviceController.SetMediaStreamPropertiesAsync(MediaStreamType.Photo, encodingProperties);
            }
        }

        /// <summary>
        /// 画像撮影呼び出し処理
        /// </summary>
        /// <returns></returns>
        private async Task TakePicture()
        {
            // デバイスエラー存在チェック
            CheckDeviceErr();

            await Task.Delay(500);

            takepicture3.Visibility = Visibility.Visible;
            await Task.Delay(500);
            Talk("さん");
            await Task.Delay(2000);

            takepicture3.Visibility = Visibility.Collapsed;
            // デバイスエラー存在チェック
            CheckDeviceErr();

            takepicture2.Visibility = Visibility.Visible;
            Talk("に");
            await Task.Delay(2000);

            takepicture2.Visibility = Visibility.Collapsed;
            // デバイスエラー存在チェック
            CheckDeviceErr();

            takepicture1.Visibility = Visibility.Visible;

            Talk("いち");
            await Task.Delay(2000);

            // デバイスエラー存在チェック
            CheckDeviceErr();

            takepicture1.Visibility = Visibility.Collapsed;
            Talk("ハイ\n\tチーーズ");
            await Task.Delay(500);

            // デバイスエラー存在チェック
            CheckDeviceErr();

            // 画像撮影メイン実行処理
            //await TakePictureExcute();
        }

        #region デバイスエラー存在チェック
        /// <summary>
        /// デバイスエラー存在チェック
        /// </summary>
        /// <remarks>
        /// デバイスエラーが発生している場合Exceptionを発行する
        /// </remarks>
        private void CheckDeviceErr()
        {
            Exception e = null;
            switch (this.DeviceError)
            {
                case DeviceErrorType.DeviceError:
                    e = new Exception("カメラデバイス認識エラー");
                    break;
                case DeviceErrorType.DeviceAccessError:
                    e = new UnauthorizedAccessException("カメラデバイスアクセスエラー");
                    break;
                case DeviceErrorType.DeviceNotPossess:
                    e = new NotPossessException("カメラデバイス占有エラー");
                    break;
                default:
                    return;
            }

            Task.Delay(500);
            throw e;
        }
        #endregion

        /// <summary>
        /// デバイスエラー処理
        /// </summary>
        /// <remarks>
        /// エラーの存在を確認した時、レスポンスページでエラー通知を行い、
        /// メインページに戻るようにする
        /// </remarks>
        /// <returns>
        /// true:エラーあり
        /// false:エラーなし
        /// </returns>
        private bool DeviceErrorExecute()
        {
            bool retValue = false;

            // デバイスエラー状態チェック
            switch (this.DeviceError)
            {
                //　写真撮影　失敗(カメラ認識不可)
                case DeviceErrorType.DeviceError:
                    MainPageErrorNavigate(ResponseId.MY_PICTURES_PHOTOGRAPHY_FAILURE_NOCAMERA);
                    retValue = true;
                    break;

                // 写真撮影　失敗(セキュリティオプションによるカメラデバイスアクセス不可)
                case DeviceErrorType.DeviceAccessError:
                    MainPageErrorNavigate(ResponseId.MY_PICTURES_PHOTOGRAPHY_FAILURE_CAMERA_ACCESS);
                    retValue = true;
                    break;

                // 写真撮影　失敗(他アプリ利用中によるカメラデバイスアクセス占有不可)
                case DeviceErrorType.DeviceNotPossess:
                    MainPageErrorNavigate(ResponseId.MY_PICTURES_PHOTOGRAPHY_FAILURE_CAMERA_POSSESS);
                    retValue = true;
                    break;

                default:
                    break;
            }

            return retValue;
        }

        /// <summary>
        /// 画像撮影メイン実行処理
        /// </summary>
        /// <returns></returns>
        private async Task TakePictureExcute()
        {
            //string imageFileName = Def.CAPTURE_IMAGE_FILENAME_PREFIX;
            //var myPictures = await Windows.Storage.StorageLibrary.GetLibraryAsync(Windows.Storage.KnownLibraryId.Pictures);
            //if (myPictures == null || myPictures.SaveFolder == null || string.IsNullOrEmpty(myPictures.SaveFolder.Path))
            //{
            //    // ごめんね写真を保存できなかったまろ
            //    MainPageErrorNavigate(ResponseId.RESPONSE_PAGE_PICTURE_FOLDER_ERROR);
            //    return;
            //}
            //string outputFolderPath = myPictures.SaveFolder.Path + "\\" + MyCloudPublicFolder;
            //string pngFileName = imageFileName + DateTime.Now.ToString(Def.MEDIA_FILENAME_DATETIME_FORMAT) + ".png";
            //// 出力フォルダが無い場合、作成する.
            //StorageFolder myCloudFolder;
            //try
            //{
            //    myCloudFolder = await myPictures.SaveFolder.GetFolderAsync(MyCloudPublicFolder);
            //}
            //catch (Exception)
            //{
            //    myCloudFolder = await myPictures.SaveFolder.CreateFolderAsync(MyCloudPublicFolder, CreationCollisionOption.GenerateUniqueName);
            //}

            //StorageFile file = await myCloudFolder.CreateFileAsync(pngFileName, CreationCollisionOption.GenerateUniqueName);

            //using (var captureStream = new InMemoryRandomAccessStream())
            //{
            //    await _mediaCapture.CapturePhotoToStreamAsync(ImageEncodingProperties.CreateJpeg(), captureStream);

            //    using (var fileStream = await file.OpenAsync(FileAccessMode.ReadWrite))
            //    {
            //        var decoder = await BitmapDecoder.CreateAsync(captureStream);
            //        var encoder = await BitmapEncoder.CreateForTranscodingAsync(fileStream, decoder);

            //        var properties = new BitmapPropertySet {
            //            { "System.Photo.Orientation", new BitmapTypedValue(PhotoOrientation.Normal, PropertyType.UInt16) }
            //        };
            //        await encoder.BitmapProperties.SetPropertiesAsync(properties);

            //        await encoder.FlushAsync();
            //    }
            //}

            //using (var stream = await file.OpenAsync(Windows.Storage.FileAccessMode.Read))
            //{
            //    BitmapImage bitmap = new BitmapImage();
            //    bitmap.SetSource(stream);
            //    imagePreview.Source = bitmap;
            //}

            //await CleanupCameraAsync();
            //CameraCapture.Visibility = Visibility.Collapsed;
            //imagePreview.Visibility = Visibility.Visible;

            //await Task.Delay(5000);
            //MainPageNavigate("photo=" + pngFileName);
        }

        /// <summary>
        /// レスポンスページを経由したメインページの表示
        /// </summary>
        /// <param name="resPrm">レスポンス画面へのパラメータ文字列</param>
        private void MainPageNavigate(string resPrm = "")
        {
            mainPage.MPageCloseCameraPreviewy();

            Task.Run(async () =>
            {
                string url = "fukumaro:PhotoTakeBack";
                if (!string.IsNullOrWhiteSpace(resPrm))
                {
                    url += "?" + resPrm;
                }

                // 画面状態を初期状態にしてトップページを表示するため、フラグを初期化する
                fukumaroData.TutorialCourse = FukumaroData.TutorialCourseType.None;
                Uri uri = new Uri(url);
                await Launcher.LaunchUriAsync(uri);
            });
        }

        /// <summary>
        /// レスポンスページにエラーを通知し後メインページの表示
        /// </summary>
        /// <param name="responseId"></param>
        private void MainPageErrorNavigate(ResponseId responseId)
        {
            MainPageNavigate("error=" + responseId);
        }

        /// <summary>
        /// Initialize or clean up the camera and our UI,
        /// depending on the page state.
        /// </summary>
        /// <returns></returns>
        private async Task SetUpBasedOnStateAsync()
        {
            if (devices == null)
            {
                return;
            }
            logger.Debug("SetUpBasedOnStateAsync");

            // Avoid reentrancy: Wait until nobody else is in this function.
            while (!_setupTask.IsCompleted)
            {
                await _setupTask;
            }

            // We want our UI to be active if
            // * We are the current active page.
            // * The window is visible.
            // * The app is not suspending.
            bool wantUIActive = _isActivePage && Window.Current.Visible && !_isSuspending;

            logger.Debug($"wantUIActive = {wantUIActive} ({_isActivePage} && {Window.Current.Visible} && {!_isSuspending})");
            if (_isUIActive != wantUIActive)
            {
                _isUIActive = wantUIActive;

                Func<Task> setupAsync = async () =>
                {
                    //logger.Debug("setupAsync " + wantUIActive);
                    //if (wantUIActive)
                    //{
                    //    if (fukumaroData.CameraDeviceId != null)
                    //    {
                    //        await SetupCameraAsync(fukumaroData.CameraDeviceId);
                    //    }
                    //}
                    //else
                    //{
                    //    await CleanupCameraAsync();
                    //}
                    //logger.Debug("setupAsync end");
                };
                _setupTask = setupAsync();
            }

            await _setupTask;
            logger.Debug("SetUpBasedOnStateAsync end");
        }
        #endregion

        private async void BtnRegister_Click(object sender, RoutedEventArgs e)
        {
            //await TakePictureExcute();
            this.InitializeDataModel();
            await Register();
        }

        public static FaceWrapperCLI.RegisterWapper registerWapper;

        private void InitializeDataModel()
        {
            string dbf = Path.Combine(ApplicationData.Current.LocalFolder.Path, "faces.db"); //"faces.db";
            string df = Path.Combine(ApplicationData.Current.LocalFolder.Path, "df.dat"); //"df.dat";
            string spFile = Path.Combine(ApplicationData.Current.LocalFolder.Path, "models\\dlib\\shape_predictor_with_glass.dat"); //"models\\dlib\\shape_predictor_with_glass.dat";

            //string dbf = Windows.Storage.ApplicationData.Current.LocalFolder.Path + "\\faces.db";
            // string df = Windows.Storage.ApplicationData.Current.LocalFolder.Path + "\\df.dat";
            //string spFile = "models\\dlib\\shape_predictor_with_glass.dat";
            string genderDf = Path.Combine(ApplicationData.Current.LocalFolder.Path, "models\\dlib\\genderdf.dat"); // "models\\dlib\\genderdf.dat";
            string genderNet = Path.Combine(ApplicationData.Current.LocalFolder.Path, "models\\caffe\\gender\\gender_net.caffemodel"); //"models\\caffe\\gender\\gender_net.caffemodel";
            string genderProto = Path.Combine(ApplicationData.Current.LocalFolder.Path, "models\\caffe\\gender\\deploy_gender.prototxt"); //"models\\caffe\\gender\\deploy_gender.prototxt";
            string ageNet = Path.Combine(ApplicationData.Current.LocalFolder.Path, "models\\caffe\\age\\age_net.caffemodel"); //"models\\caffe\\age\\age_net.caffemodel";
            string ageProto = Path.Combine(ApplicationData.Current.LocalFolder.Path, "models\\caffe\\age\\deploy_age.prototxt"); //"models\\caffe\\age\\deploy_age.prototxt";
            string imgNet = Path.Combine(ApplicationData.Current.LocalFolder.Path, "models\\dlib\\dlib_face_recognition_resnet_model_v1.dat"); //"models\\dlib\\dlib_face_recognition_resnet_model_v1.dat";

            logger.Debug($"set option dbf:{dbf}");
            logger.Debug($"set option df:{df}");
            logger.Debug($"set option spFile:{spFile}");

            //visOption = new VisualizationOption { ShowBoundingBox = false, ShowEmotion = false, ShowAge = false, ShowGender = false, ShowName = false, ShowGroup = false };
            ConfigOption configOption = new ConfigOption
            {
                DBFile = dbf,
                SVMFaceModel = df,
                ShapeModel = spFile,
                ImageNetModel = imgNet,
                GenderSVMModel = genderDf,
                GenderNetModel = genderNet,
                GenderProto = genderProto,
                AgeModel = ageNet,
                AgeProto = ageProto
            };

            RecognitionOption recogniOption = new RecognitionOption { Tracking = false, Face = true, Emotion = false, Age = false, Gender = false, GenderType = ModelType.SVM, FaceInterval = 25, EmotionInterval = 2, GenInterval = 30, AgeInterval = 35 };

            IList<ErrorInfo> errors = new List<ErrorInfo>();
            registerWapper = new FaceWrapperCLI.RegisterWapper(configOption, errors);
            registerWapper.Logger += RegisterWapper_Logger;
            if (errors.Count > 0)
            {
                textStatus.Text = $"Status: init RegisterWapper error {errors[0].Message}";
            }
            else
            {
                textStatus.Text = $"Status: init RegisterWapper success!!";
            }
        }

        private async Task Register()
        {
            List<FaceData> faceDatas = new List<FaceData>();
            for (int i = 0; i < faceImages.Count; i++)
            {
                faceDatas.Add(
                        new FaceData
                        {
                            FrameImage = SoftwareBitmap.Copy(faceImages[i]),
                            FaceBB = new FaceWrapperCLI.FaceBox
                            {
                                Left = (int)facesDetected[i].FaceBox.X,
                                Right = (int)(facesDetected[i].FaceBox.X + facesDetected[i].FaceBox.Width),
                                Top = (int)facesDetected[i].FaceBox.Y,
                                Bottom = (int)(facesDetected[i].FaceBox.Y + facesDetected[i].FaceBox.Height)
                            }
                        });
            }


            UserData userData = new UserData
            {
                UserName = fukumaroData.FaceName,
                FaceDatas = faceDatas,
                FrameImage = SoftwareBitmap.Copy(faceImages[0])
            };

            FaceInfo userInfor = new FaceInfo();
            IList<ErrorInfo> error = new List<ErrorInfo>();
            ErrorCode errorCode = registerWapper.CreateFace(userData, userInfor, error);
            textStatus.Text = "Status:ok";
            if (errorCode != ErrorCode.SUCCESS)
            {
                textStatus.Text = $"Status: error lErrorCode:{error[0].lErrorCode}  ClassName:{error[0].ClassName}  FunctionName:{error[0].FunctionName} Message:{error[0].Message}";
                return;
            }
            error = new List<ErrorInfo>();
            errorCode = registerWapper.Train(error);
            if (errorCode != ErrorCode.SUCCESS)
            {
                textStatus.Text = $"Status: error lErrorCode:{error[0].lErrorCode}  ClassName:{error[0].ClassName}  FunctionName:{error[0].FunctionName} Message:{error[0].Message}";
                return;
            }
            else
            {
                textStatus.Text = $"Status: register success";
                error = new List<ErrorInfo>();
                registerWapper.Terminate(error);
                await Task.Delay(1000);
                await CleanupCameraAsync();
                await Task.Delay(5000);
                Navigate(typeof(TopPage));
            }



            
            logger.Debug($"Initialize---->end");
        }

        private void RegisterWapper_Logger(string message)
        {
            logger.Debug($"RegisterWapper_Logger: {message}");
        }

        List<SoftwareBitmap> faceImages = new List<SoftwareBitmap>();
        IList<Windows.Media.FaceAnalysis.DetectedFace> facesDetected = new List<Windows.Media.FaceAnalysis.DetectedFace>();

       

        /// <summary>
        /// BtnCapture_Click
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void BtnCapture_Click(object sender, RoutedEventArgs e)
        {
            VideoEncodingProperties videoProperties = _mediaCapture.VideoDeviceController.GetMediaStreamProperties(MediaStreamType.Photo) as VideoEncodingProperties;

            //var lowLagCapture = await _mediaCapture.PrepareLowLagPhotoCaptureAsync(ImageEncodingProperties.CreateUncompressed(MediaPixelFormat.Nv12));
            //var capturedPhoto = await lowLagCapture.CaptureAsync();
            //var softwareBitmap = capturedPhoto.Frame.SoftwareBitmap;

            //await lowLagCapture.FinishAsync();

            //VideoFrame previewFrame = new VideoFrame(InputPixelFormat, (int)this.videoProperties.Width, (int)this.videoProperties.Height)
            const BitmapPixelFormat InputPixelFormat = BitmapPixelFormat.Nv12;
            using (VideoFrame previewFrame = new VideoFrame(InputPixelFormat, (int)videoProperties.Width, (int)videoProperties.Height))
            {
                await this._mediaCapture.GetPreviewFrameAsync(previewFrame);
                // The returned VideoFrame should be in the supported NV12 format but we need to verify this.
                if (FaceDetector.IsBitmapPixelFormatSupported(previewFrame.SoftwareBitmap.BitmapPixelFormat))
                {
                    SoftwareBitmap originalBitmap = null;
                    originalBitmap = SoftwareBitmap.Convert(previewFrame.SoftwareBitmap, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);

                    var faces = await faceTracker1.DetectFacesAsync(previewFrame.SoftwareBitmap);
                    if (faces == null)
                    {
                        textStatus.Text = $"Status: error FaceDetector";
                        //await lowLagCapture.FinishAsync();
                        return;
                    }

                    if (faces.Count == 0)
                    {
                        textStatus.Text = $"Status: No FaceDetector";
                        //await lowLagCapture.FinishAsync();
                        return;
                    }

                    if (faces.Count > 1)
                    {
                        textStatus.Text = $"Status: many face FaceDetector";
                        //await lowLagCapture.FinishAsync();
                        return;
                    }

                    faceImages.Add(originalBitmap);
                    facesDetected.Add(faces[0]);
                    SoftwareBitmap croppedBitmap = await CropBitmap.CorpSoftwareBitmap(originalBitmap, faces[0]);
                    SoftwareBitmap imageBitmap = SoftwareBitmap.Convert(croppedBitmap, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
                    var softwareBitmapSource = new SoftwareBitmapSource();
                    await softwareBitmapSource.SetBitmapAsync(imageBitmap);
                    switch (faceImages.Count)
                    {
                        case 1:
                            image1.Source = softwareBitmapSource;
                            break;
                        case 2:
                            image2.Source = softwareBitmapSource;
                            break;
                        case 3:
                            image3.Source = softwareBitmapSource;
                            break;
                        case 4:
                            image4.Source = softwareBitmapSource;
                            break;
                        case 5:
                            image5.Source = softwareBitmapSource;
                            break;
                    }
                }
            }
        }
    }

    //#region カメラデバイス占有失敗エクセプションクラス
    ///// <summary>
    ///// カメラデバイス占有失敗エクセプションクラス
    ///// </summary>
    //internal class NotPossessException : Exception
    //{
    //    #region コンストラクタ
    //    /// <summary>
    //    /// コンストラクタ
    //    /// </summary>
    //    public NotPossessException() : base() { }
    //    /// <summary>
    //    /// コンストラクタ
    //    /// </summary>
    //    /// <param name="message"></param>
    //    public NotPossessException(string message) : base(message) { }
    //    /// <summary>
    //    /// コンストラクタ
    //    /// </summary>
    //    /// <param name="message"></param>
    //    /// <param name="innerException"></param>
    //    public NotPossessException(string message, Exception innerException) : base(message, innerException) { }
    //    #endregion
    //}
    //#endregion
}
