#include "stdafx.h"
#include "Ann.h"

Ann::Ann(string SDP, int my_port)
{
	av_log_set_level(0);
	av_register_all();
	avcodec_register_all();
	avfilter_register_all();
	avformat_network_init();

	remote_ip_ = MakeRemoteIP(SDP);
	cout << "\nip=" << remote_ip_;
	remote_port_ = stoi(MakeRemotePort(SDP));
	cout << "     port=" << remote_port_;
	my_port_ = my_port;
	rtp_hdr.rtp_config();
	sock.reset(new udp::socket(io_service_, udp::endpoint(udp::v4(), my_port_)));
	endpt = udp::endpoint(boost::asio::ip::address::from_string(remote_ip_), remote_port_);
}

void Ann::openFile(string filename)
{
	int m_lastError = 0;

	avformat_open_input(&ifcx, filename.c_str(), NULL, NULL);
	avformat_find_stream_info(ifcx, NULL);
	//Выбор индекса потока
	/*for (unsigned i = 0; idxStream_ == -1 && i < ifcx->nb_streams; ++i)
	idxStream_ = ifcx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO
	? i : -1;*/
	AVCodec *input_codec = avcodec_find_decoder(ifcx->streams[0]->codec->codec_id);
	avcodec_open2(ifcx->streams[0]->codec, input_codec, nullptr);
	if (!ifcx->streams[0]->codec->channel_layout)
		ifcx->streams[0]->codec->channel_layout = av_get_default_channel_layout(ifcx->streams[0]->codec->channels);
}
void Ann::openRTP()
{
	auto strRTP = str(boost::format("rtp://%1%:%2%?localport=%3%") % remote_ip_ %remote_port_ %(my_port_-1000));
	cout << "\nrtp to: " << strRTP;
	avformat_alloc_output_context2(&out_ifcx, nullptr, "rtp", strRTP.c_str());
	avio_open(&out_ifcx->pb, strRTP.c_str(), AVIO_FLAG_WRITE);

	AVCodecID idCodec = AV_CODEC_ID_PCM_ALAW;
	AVCodec *output_codec = avcodec_find_encoder(idCodec);

	auto strmOut = avformat_new_stream(out_ifcx, output_codec);
	strmOut->time_base = { 1, 8000 };
	out_iccx = strmOut->codec;

	out_iccx->channels = 1;
	out_iccx->channel_layout = av_get_default_channel_layout(out_iccx->channels);
	out_iccx->sample_fmt = output_codec->sample_fmts[0];
	out_iccx->sample_rate = 8000;
	out_iccx->bit_rate = 8000;
	out_iccx->time_base = { 1, out_iccx->sample_rate };

	avcodec_open2(strmOut->codec, output_codec, nullptr);
}
void Ann::Send(string file)
{
	filename_ = file;
	cout << "\n"<<filename_;
	openFile(file);
	cout << "\nfile opened";
	openRTP();
	cout << "\nRTP opened";
	Run();
}
void Ann::Run()
{
	//avformat_write_header(out_ifcx, NULL);
	int data_present = 0;
	cout << "\nRun init done";
	//avformat_write_header(out_ifcx, NULL);
	while (true)
	{
		data_present = 0;
		AVFrame *frame = NULL;
		frame = av_frame_alloc();
		cout << "\n1";
		decode_audio_frame(frame, &data_present);
		cout << "\n2";
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		cout << "\n3";
		encode_audio_frame(frame, &data_present);
		cout << "\n4";
		av_frame_free(&frame);
		cout << "\n5";
	}
}
int Ann::decode_audio_frame(AVFrame *frame, int *data_present)
{
	/** Packet used for temporary storage. */
	AVPacket input_packet;
	SHP_CAVPacket2 send;
	int error;
	init_packet(&input_packet);
	cout << "\n1.1";
	if ((error = av_read_frame(ifcx, &input_packet)) < 0) {
		cout << "\nerrrrrrrr";
		/** If we are the the end of the file, flush the decoder below. */
		/*add end of file*/
	}
	else{ cout << "\nnoerr"; }
	send.reset(new CAVPacket2(input_packet.size));
	memcpy(send->data, input_packet.data, input_packet.size);
	cout << "\n1.2 data_present=" << *data_present << " data=" << send.get()->data << "\nsize=" << input_packet.size;
	avcodec_decode_audio4(iccx, frame, data_present, send.get());
	cout << "\n1.3";
	av_free_packet(&input_packet);
	cout << "\n1.4";
	return 0;
}
int Ann::encode_audio_frame(AVFrame *frame, int *data_present)
{
	/** Packet used for temporary storage. */
	AVPacket output_packet;
	SHP_CAVPacket2 send;
	//	int error;
	init_packet(&output_packet);

	avcodec_encode_audio2(out_iccx, &output_packet, frame, data_present);

	/*if (*data_present)
	{
	if ((error = av_write_frame(out_ifcx, &output_packet)) < 0)//interleaved_
	{
	cout << "\nerror = av_write_frame";
	av_free_packet(&output_packet);
	return error;
	}
	cout << "\nwrite frame";
	av_free_packet(&output_packet);
	}*/
	send.reset(new CAVPacket2(output_packet.size + 12));
	rtp_hdr.rtp_modify();
	memcpy(send->data, (uint8_t*)&rtp_hdr.header, 12);
	memcpy(send->data + 12, output_packet.data, output_packet.size);
	sock->send_to(boost::asio::buffer(send->data, send->size), endpt);
	send->free();
	av_free_packet(&output_packet);
	return 0;
}
void Ann::init_packet(AVPacket *packet)
{
	av_init_packet(packet);
	/** Set the packet data and size so that it is recognized as being empty. */
	packet->data = NULL;
	packet->size = 0;
}