import { Component, EventEmitter, OnInit, Output } from '@angular/core';
import { FormControl, FormGroup, Validators } from '@angular/forms';
import { MdChipEvent } from '@angular/material/material';
import { ConnectionMeta } from 'app/classes/connnection-meta';
import { ConnectionHistoryService } from 'app/services/connection-history/connection-history.service';
import { Observable } from 'rxjs/Observable';

@Component({
  selector: 'app-new-connection',
  templateUrl: './new-connection.component.html',
  styleUrls: ['./new-connection.component.scss']
})
export class NewConnectionComponent {
  @Output() public connection = new EventEmitter<ConnectionMeta>();

  protected filteredPastConnections: ConnectionMeta[] = [];
  protected connectionForm = new FormGroup({
    address: new FormControl('', [Validators.required]),
    name: new FormControl('', [Validators.required])
  });

  constructor(private connectionHistory: ConnectionHistoryService) {
    Observable.combineLatest(
      this.connectionHistory.get(),
      this.connectionForm.get('address').valueChanges.startWith(''),
      this.connectionForm.get('name').valueChanges.startWith('')
    )
      .filter((val: [ConnectionMeta[], string, string]) => val[0] !== null)
      .subscribe(val => this.filterHistory(val));
  }

  protected connect() {
    if (this.connectionForm.status === 'VALID') {
      console.log('connect');
      const newConnection = new ConnectionMeta(this.connectionForm.get('name').value, this.connectionForm.get('address').value)
      this.clearForm();
      this.connectionHistory.store(newConnection);
      this.connection.emit(newConnection);
    }
  }

  protected historySelected(connection: ConnectionMeta) {
    this.connectionForm.setValue({ address: connection.address, name: connection.name });
  }

  protected historyDeselected(connection: ConnectionMeta) {
    console.log('deselected: ', connection.name);
    // Only clear fields if they are the same as the chip, other wise they are being edited by the person
    if (this.historySameAsInput(connection)) {
      this.clearForm();
    }
  }

  protected historySameAsInput(connection: ConnectionMeta): boolean {
    return this.connectionForm.get('address').value === connection.address && this.connectionForm.get('name').value === connection.name;
  }

  protected removeHistory(connection: ConnectionMeta) {
    this.connectionHistory.remove(connection.name);
    if (this.historySameAsInput(connection)) {
      this.clearForm();
    }
  }

  private clearForm() {
    this.connectionForm.setValue({ address: '', name: '' });
  }

  private filterHistory(val: [ConnectionMeta[], string, string]) {
    const [pastConnections, address, name] = val;
    // need to add sort as well
    this.filteredPastConnections = pastConnections
      // filter by both fields
      .filter((connection: ConnectionMeta) => {
        // if both fields are empty we should show the whole history
        const emptyFields = (address === '' && name === '');
        if (emptyFields) {
          return true;
        } else {
          // if either one of the fields contain data, we should filter by them both, exlucing
          const addressFilter = (address === '') ? false : connection.address.toLowerCase().includes(address.toLowerCase());
          const nameFilter = (name === '') ? false : connection.name.toLowerCase().includes(name.toLowerCase());
          return addressFilter || nameFilter;
        }
      })
      .sort((a: ConnectionMeta, b: ConnectionMeta) => {
        const addressA = a.address.toLowerCase();
        const addressB = b.address.toLowerCase();
        return +(addressA > addressB) || +(addressA === addressB) - 1;
      })
      .sort((a: ConnectionMeta, b: ConnectionMeta) => {
        const nameA = a.name.toLowerCase();
        const nameB = b.name.toLowerCase();
        return +(nameA > nameB) || +(nameA === nameB) - 1;
      });
  }
}
