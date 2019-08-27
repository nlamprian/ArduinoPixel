package ln.paign10.arduinopixel;

import java.util.ArrayList;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.NumberPicker;

public class IpDialog extends DialogFragment {

	public interface DialogListener {
		public void setNpValues(ArrayList<NumberPicker> np);

		public void setUri(ArrayList<NumberPicker> np);
	}

	ArrayList<NumberPicker> mNpList = new ArrayList<NumberPicker>(5);
	private NumberPicker np1, np2, np3, np4, np5;
	private DialogListener mListener;

	@Override
	public void onStart() {
		super.onStart();

		if (getDialog() == null)
			return;

		// getDialog().getWindow().setLayout(520, 400);
		// getDialog().getWindow().getAttributes().width = 520;
	}

	@Override
	public Dialog onCreateDialog(Bundle savedInstanceState) {
		AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
		mListener = (DialogListener) getActivity();

		LayoutInflater inflater = getActivity().getLayoutInflater();
		View layout = inflater.inflate(R.layout.ip_dialog_layout, null);

		np1 = (NumberPicker) layout.findViewById(R.id.numberPicker1);
		np2 = (NumberPicker) layout.findViewById(R.id.numberPicker2);
		np3 = (NumberPicker) layout.findViewById(R.id.numberPicker3);
		np4 = (NumberPicker) layout.findViewById(R.id.numberPicker4);
		np5 = (NumberPicker) layout.findViewById(R.id.numberPicker5);
		np1.setMinValue(0);
		np2.setMinValue(0);
		np3.setMinValue(0);
		np4.setMinValue(0);
		np5.setMinValue(1);
		np1.setMaxValue(255);
		np2.setMaxValue(255);
		np3.setMaxValue(255);
		np4.setMaxValue(255);
		np5.setMaxValue(65535);

		mNpList.add(np1);
		mNpList.add(np2);
		mNpList.add(np3);
		mNpList.add(np4);
		mNpList.add(np5);

		mListener.setNpValues(mNpList);

		builder.setView(layout)
				.setTitle("Choose Host IP address and Port number")
				.setNegativeButton(R.string.cancel,
						new DialogInterface.OnClickListener() {
							@Override
							public void onClick(DialogInterface dialog, int id) {
								IpDialog.this.getDialog().cancel();
							}
						})
				.setPositiveButton(R.string.ok,
						new DialogInterface.OnClickListener() {
							@Override
							public void onClick(DialogInterface dialog, int id) {
								mListener.setUri(mNpList);
							}
						});
		return builder.create();
	}
}
